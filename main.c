#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"
#include "analizer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/raw_array.h"


void prompt() { printf("netdump > "); };

int main(int argv, char *argc[]) {
	if (0 != geteuid()) raise(USER_NOT_ROOT_ERROR, 1);	/* root access is needed in order to execute pcap packet scan */

	buffer buff = { .content = NULL, .len = 0 };
	command cmd = { .n_args = 0, .label = NULL, .hashes = 0, .args = NULL };
	raw_array packets = { .values = NULL, .allocated = 0, .len = 0 };

	// deallocate char string in buffer (where?)
	// write the destroy_cmd() function
	
	// find a way to manage error message precedence. (e.g. if print - n (which is wrong), print WRONG_OPTION_FORMAT_ERROR)
	// and go to the next netdump iteration (netdump > ). Skip errors like (INDEX_ERROR, ecc...). The WRONG_OPTION_FORMAT_ERROR
	// should abort the command and instantaneously go to the next while iteration

	while(1) {
		reset_cmd(&cmd);	/* ensure that cmd structure is empty at each iteration */
		prompt();
		populate(&buff);	/* populate() alredy overrides previously buffered data */
		create_cmd_from_buff(&cmd, &buff);

		if (0 == buff.len) continue;

		if (0 != execute_command(&cmd, &packets)) {
			raise(UNKNOWN_COMMAND_ERROR, 0, cmd.label);
			continue;
		}
	}

	return 0;
}