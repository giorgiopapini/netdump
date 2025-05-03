#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>

#include "command_handler.h"
#include "status_handler.h"
#include "utils/buffer.h"
#include "utils/raw_array.h"
#include "utils/circular_linked_list.h"
#include "utils/packet.h"
#include "utils/string_utils.h"
#include "utils/visualizer.h"
#include "utils/formats.h"

/*
	TODO (optional):	(prevent the shift + arrow_up to print ;2A in terminal) (in general prevent shift + arrow printing)

	TODO: 	Add a timeout error (and a flag to deactivate the timeout feature) when analizing for packets. If no packets are
			received in a specific amount of time, than raise timeout error

	TODO:	Implement the 'print_ip_options(const uint8_t *pkt)' function inside of ip.c module
	
	TODO:	Add a message to notify the user when a protocol header is not supported and can't be dissected and printed (or visualized).
			The save command still works btw, even if the packet can't be parsed

	TODO: 	I need the source and destination IPs for the UDP checksum, but they're unavailable 
			by the time I reach print_udp_hdr(). How can I solve the problem?

	TODO:	Command to compile shared library
			gcc -fPIC -shared -o diss_prova.so diss_prova.c -lnetdump

	TODO: 	Add a way to load any .so file in the program given one or more directories to look at
			(like: "home/dissectors" or "home/dissectors/" look inside the directory and load any .so file)
*/

void deallocate_heap(command *, raw_array *, circular_list *, shared_libs *, custom_dissectors *);
cmd_retval run(buffer *, command *, raw_array *, circular_list *, shared_libs *, custom_dissectors *);
void prompt(void);


int main(void) {
	fd_set readfds;
	cmd_retval retval;
    int ret;
	struct termios original, term;

	buffer buff = { 0 };
	command cmd = { 0 };
	raw_array packets = { 0 };
	circular_list history = { 0 };
	shared_libs libs = { 0 };
	custom_dissectors custom_diss = { 0 };

    tcgetattr(STDIN_FILENO, &original);
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
	
	retval = RET_UNKNOWN;
	prompt();
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);

        if (-1 == ret) {
			if (EINTR == errno) break;
			else raise_error(SELECT_FAILED_ERROR, 0, NULL, __FILE__, strerror(errno));
		}
		else retval = run(&buff, &cmd, &packets, &history, &libs, &custom_diss);
		if (RET_EXIT == retval) break;
    }
	
	tcsetattr(STDIN_FILENO, TCSANOW, &original);
	deallocate_heap(&cmd, &packets, &history, &libs, &custom_diss);
	return 0;
}


void deallocate_heap(
	command *cmd,
	raw_array *packets,
	circular_list *history,
	shared_libs *libs,
	custom_dissectors *custom_diss
) {
	destroy_list(history, destroy_buffer);
	reset_cmd(cmd);
	reset_arr(packets, destroy_packet);
	destroy_shared_libs(libs);
	destroy_custom_dissectors(custom_diss);
}

cmd_retval run(
	buffer *buff, 
	command *cmd, 
	raw_array *packets, 
	circular_list *history,
	shared_libs *libs,
	custom_dissectors *custom_diss
) {
	cmd_retval retval = RET_UNKNOWN;
	int pressed_key = populate(buff, history);

	if (-1 == pressed_key) return RET_UNKNOWN;  /* if an error occoured, exit function */
	if (ENTER_KEY != pressed_key) return RET_UNKNOWN;  /* if enter is not pressed, than do not start executing partial cmd */

	reset_cmd(cmd);  /* ensure that cmd structure is empty when new command entered */

	if (buff->len == 0 || '\0' == buff->content[0]) {
		prompt();
		return RET_UNKNOWN;
	}

	if (NULL != history->curr) history->curr = history->head;  /* at each cmd execution reset history->curr position */

	/* if history.head != NULL, buffer not equal to last buffer in history and buffer longer than 0 than push to history */
	if (NULL != history->head) {
		if (!compare_buffers(history->head->prev->content, buff) && buff->len > 0) {
			push_node(history, create_node(copy_to_heap(buff)), MAX_BUFFER_LEN, destroy_buffer);
		}
	}
	else push_node(history, create_node(copy_to_heap(buff)), MAX_BUFFER_LEN, destroy_buffer);

	if (0 == check_buffer_status(buff)) {
		if (0 == create_cmd_from_buff(cmd, buff)) {
			retval = execute_command(cmd, packets, libs, custom_diss);
			if (RET_UNKNOWN == retval) raise_error(UNKNOWN_COMMAND_ERROR, 0, UNKNOWN_COMMAND_HINT, cmd->label);
		}
	}

	clear_buffer(buff);
	if (RET_EXIT != retval) prompt();
	return retval;
}

void prompt(void) { 
	printf(PROMPT_STRING); 
	fflush(stdout);
}