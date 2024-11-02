#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/raw_array.h"


void prompt() { printf("netdump > "); };

int main(int argv, char *argc[]) {
	if (0 != geteuid()) raise_error(USER_NOT_ROOT_ERROR, 1, NULL);	/* root access is needed in order to execute pcap packet scan */

	buffer buff = { .content = NULL, .len = 0 };
	command cmd = { .n_hashes = 0, .label = NULL, .hashes = 0, .args = NULL };
	raw_array packets = { .values = NULL, .allocated = 0, .len = 0 };

	while(1) {
		reset_cmd(&cmd);	/* ensure that cmd structure is empty at each iteration */
		prompt();
		populate(&buff);	/* populate() alredy overrides previously buffered data */
		
		if (0 == create_cmd_from_buff(&cmd, &buff)) continue;
		if (0 == buff.len) continue;

		if (0 != execute_command(&cmd, &packets)) {
			raise_error(UNKNOWN_COMMAND_ERROR, 0, NULL, cmd.label);
			continue;
		}
	}

	return 0;
}