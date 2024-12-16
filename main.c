#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils/buffer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/raw_array.h"
#include "utils/circular_linked_list.h"
#include "utils/packet.h"

#define PROMPT_STRING "netdump > "

#include "utils/visualizer.h"

/*
	TODO:	Manage multiline terminal string. When left arrow is pressed at start of line x, it doesnt 'teleport' to the end
			of line (x - 1). pos variable does not need to become a 2D variable (maybe). It is obvious that when the eos is
			reached it has to go up or down by maximum 1 line. Find a way to get terminal cols and absolute position of cursor
			(netdump > ) characters need to be counted in some way

	TODO (optional):	(prevent the shift + arrow_up to print ;2A in terminal) (in general prevent shift + arrow printing)

	TODO:	Add a way to read from a PCAP file. Maybe this might be useful also to test uncommon protocols (dissection and
			printing)

	TODO:	Add a parameter to let the user chose the device for the packet sniffing
	TODO:	Add a way to save the scanned packets into a pcap file (maybe a new command called 'save'?)
*/

void deallocate_heap(command *cmd, raw_array *packets, circular_list *history) {
	destroy_list(history, destroy_buffer);
	reset_cmd(cmd);
	reset_arr(packets, destroy_packet);
}

void prompt() { printf(PROMPT_STRING); };

int main(int argv, char *argc[]) {
	if (0 != geteuid()) raise_error(USER_NOT_ROOT_ERROR, 1, NULL);	/* root access is needed in order to execute pcap packet scan */

	buffer buff = { .len = 0 };
	command cmd = { .n_hashes = 0, .label = NULL, .hashes = 0, .args = NULL };
	raw_array packets = { .values = NULL, .allocated = 0, .len = 0 };
	circular_list history = { .head = NULL, .len = 0 };

	while(1) {
		reset_cmd(&cmd);	/* ensure that cmd structure is empty at each iteration */
		prompt();

		/* if CTRL+C is pressed, than deallocate previously allocated heap memory and exit program */
		if (0 != populate(&buff, &history)) {
			deallocate_heap(&cmd, &packets, &history);
			exit(EXIT_SUCCESS);
		}

		if (buff.len == 0 || '\0' == buff.content[0]) continue;

		/* if history.head != NULL, buffer not equal to last buffer in history and buffer longer than 0 than push to history */
		if (history.head != NULL) {
			if (!compare_buffers(history.head->prev->content, &buff) && buff.len > 0) {
				push_node(&history, create_node(copy_to_heap(&buff)), MAX_BUFFER_LEN, destroy_buffer);
			}
		}
		else push_node(&history, create_node(copy_to_heap(&buff)), MAX_BUFFER_LEN, destroy_buffer);

		if (0 != check_buffer_status(&buff)) continue;
		if (0 != create_cmd_from_buff(&cmd, &buff)) continue;
		if (0 != execute_command(&cmd, &packets, &history)) raise_error(UNKNOWN_COMMAND_ERROR, 0, NULL, cmd.label);
	}

	return 0;
}