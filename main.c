#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/raw_array.h"
#include "utils/circular_linked_list.h"


/*
	TODO:	Remake the raw_array struct. I need to save not simply void pointers to byte array (packet).
			I need to save (at least) also the timestamp, and size of packet
	
	
	TODO:	Manage multiline terminal string. When left arrow is pressed at start of line x, it doesnt 'teleport' to the end
			of line (x - 1)	
	

	TODO:	Solve the issue "command  arg" is valid and 'arg' is recognized as argument even if the '-' separator is not there
	TODO:	Understand how to manage byte order of fields given different protocols in 'analize.c' module
			(not every protocol has to be read in NETWORK BYTE ORDER?). 
*/


void prompt() { printf("netdump > "); };

int main(int argv, char *argc[]) {
	if (0 != geteuid()) raise_error(USER_NOT_ROOT_ERROR, 1, NULL);	/* root access is needed in order to execute pcap packet scan */

	buffer buff = { .len = 0 };
	command cmd = { .n_hashes = 0, .label = NULL, .hashes = 0, .args = NULL };
	raw_array packets = { .values = NULL, .allocated = 0, .len = 0 };
	circular_list history = { .head = NULL, .len = 0 };
	
	while(1) {
		reset_cmd(&cmd);	/* ensure that cmd structure is empty at each iteration */
		prompt();
		populate(&buff, &history);	/* populate() alredy overrides previously buffered data */
		
		/* if the max len is hit, the function deallocates the node and what is inside of it (e.g. buffer) */
		if(buff.len > 0) push_node(&history, create_node(copy_to_heap(&buff)), MAX_BUFFER_LEN, destroy_buffer);

		if (0 != check_buffer_status(&buff)) continue;	/* if an error occoured, than skip to next iteration */
		if (0 != create_cmd_from_buff(&cmd, &buff)) continue;
		if (buff.len == 0) continue;

		if (0 != execute_command(&cmd, &packets)) raise_error(UNKNOWN_COMMAND_ERROR, 0, NULL, cmd.label);
	}

	return 0;
}