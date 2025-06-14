#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>

#include "command_handler.h"
#include "status_handler.h"
#include "utils/buffer.h"
#include "utils/raw_array.h"
#include "utils/packet.h"
#include "utils/formats.h"
#include "utils/circular_linked_list.h"
#include "utils/command.h"
#include "utils/shared_lib.h"
#include "utils/terminal_handler.h"
#include "utils/custom_dissectors.h"
#include "protocols/proto_tables_handler.h"

/*
	TODO (optional):	(prevent the shift + _arrow_up to print ;2A in terminal) (in general prevent shift + arrow printing)

	TODO: 	Add a timeout error (and a flag to deactivate the timeout feature) when analizing for packets. If no packets are
			received in a specific amount of time, than raise timeout error

	TODO:	Implement the 'print_ip_options(const uint8_t *pkt)' function inside of ip.c module
	
	TODO:	Add a message to notify the user when a protocol header is not supported and can't be dissected and printed (or visualized).
			The save command still works btw, even if the packet can't be parsed

	TODO: 	I need the source and destination IPs for the UDP checksum, but they're unavailable 
			by the time I reach _print_udp_hdr(). How can I solve the problem?

	TODO:	Command to compile shared library
			gcc -fPIC -shared -o diss_prova.so diss_prova.c -lnetdump

	TODO: 	Fix the "non literal string" warning in vfprintf(stderr, (const char *)error_formats[code], args);
			in 'status_handler.c'

	TODO: 	strtok modify the string on which is applied, check if this is a safe behaviour everywhere in the codebase
			otherwise fix creating a copy using strdup of the string, than running strtok

	TODO: 	insted of proto_table_id, a standard int value is used across the codebase, check if it is good (also thinking
			about the shared lib (libpcap) for custom dissectors)

	TODO: 	Return value from dissector_add to avoid proceeding the execution of the command even if an error occoured

	TODO: 	Fix the string overflowing the terminal cols bug

	TODO: 	Implement a command like "stats" to print statistics about a determined scanned session

			(e.g.) Capture Statistics:
				-------------------
				Total packets captured: 5347
				Duration: 120.3 seconds
				Average packets/sec: 44.45

				Protocol Breakdown:
				- TCP: 4012 packets (75%)
				- UDP: 982 packets (18%)
				- ICMP: 353 packets (7%)

				Top 5 Source IPs:
				1. 192.168.0.10 (1240 packets)
				2. 10.0.0.5 (902 packets)
				...

				Top 5 Destination Ports:
				1. 443 (HTTPS) - 1894 packets
				2. 53 (DNS) - 850 packets
				3. 22 (SSH) - 312 packets
*/

static void _deallocate_heap(
	command *cmd,
	raw_array *packets,
	circular_list *history,
	shared_libs *libs,
	custom_dissectors *custom_diss
);
static cmd_retval _run(
	buffer *buff, 
	command *cmd, 
	raw_array *packets, 
	circular_list *history,
	shared_libs *libs,
	custom_dissectors *custom_diss
);
static void _prompt(void);


int main(void) {
	fd_set readfds;
	cmd_retval retval;
    int ret;

	buffer buff = { 0 };
	command cmd = { 0 };
	raw_array packets = { 0 };
	circular_list history = { 0 };
	shared_libs libs = { 0 };
	custom_dissectors custom_diss = { 0 };

	load_proto_hashmaps();
	enable_raw_mode();
    atexit(restore_terminal_mode);
	
	retval = RET_UNKNOWN;
	_prompt();
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);

        if (-1 == ret) {
			if (EINTR == errno) break;
			else raise_error(SELECT_FAILED_ERROR, 0, NULL, __FILE__, strerror(errno));
		}
		else retval = _run(&buff, &cmd, &packets, &history, &libs, &custom_diss);
		if (RET_EXIT == retval) break;
    }
	
	restore_terminal_mode();  /* maybe superfluous? */
	_deallocate_heap(&cmd, &packets, &history, &libs, &custom_diss);
	return 0;
}


static void _deallocate_heap(
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
	destroy_proto_hashmaps();
}

static cmd_retval _run(
	buffer *buff, 
	command *cmd, 
	raw_array *packets, 
	circular_list *history,
	shared_libs *libs,
	custom_dissectors *custom_diss
) {
	cmd_retval retval = RET_UNKNOWN;
	int pressed_key;

	pressed_key = populate(buff, history);
	if (-1 == pressed_key) return RET_UNKNOWN;  /* if an error occoured, exit function */
	if (ENTER_KEY != pressed_key) return RET_UNKNOWN;  /* if enter is not pressed, than do not start executing partial cmd */

	reset_cmd(cmd);  /* ensure that cmd structure is empty when new command entered */

	CHECK_NULL_EXIT(buff);
	CHECK_NULL_EXIT(buff->content);
	if (buff->len == 0 || '\0' == buff->content[0]) {
		_prompt();
		return RET_UNKNOWN;
	}

	CHECK_NULL_EXIT(history);
	if (NULL != history->curr) history->curr = history->head;  /* at each cmd execution reset history->curr position */

	/* if history.head != NULL, buffer not equal to last buffer in history and buffer longer than 0 than push to history */
	if (NULL != history->head) {
		CHECK_NULL_EXIT(history->head->prev);
		if (!compare_buffers(history->head->prev->content, buff) && buff->len > 0) {
			push_node(history, create_node(copy_to_heap(buff)), MAX_BUFFER_LEN, destroy_buffer);
		}
	}
	else push_node(history, create_node(copy_to_heap(buff)), MAX_BUFFER_LEN, destroy_buffer);

	if (0 == check_buffer_status(buff)) {
		if (0 == create_cmd_from_buff(cmd, buff)) {
			retval = execute_command(cmd, packets, libs, custom_diss);
			if (RET_UNKNOWN == retval)
				raise_error(UNKNOWN_COMMAND_ERROR, 0, UNKNOWN_COMMAND_HINT, cmd->label);
		}
	}

	clear_buffer(buff);
	if (RET_EXIT != retval) _prompt();
	return retval;
}

static void _prompt(void) { 
	printf(PROMPT_STRING); 
	fflush(stdout);
}