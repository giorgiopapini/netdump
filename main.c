#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/easycli.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/raw_array.h"
#include "utils/packet.h"
#include "utils/command.h"
#include "utils/shared_lib.h"
#include "utils/custom_dissectors.h"
#include "utils/hierachy.h"
#include "utils/formats.h"
#include "protocols/proto_tables_handler.h"

/*
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

	TODO: 	Implement a command like "stats" to print statistics about a determined scanned session
			(Search for Wireshark statistics tool for insights)
			(STATS AND ANALIZE SHOULD BE STRICTLY SEPARATED, STATS SHOULDN'T BE RUN AT THE SAME TIME OF THE SCANNING.
			OTHERWISE IT WILL SLOW THINGS DOWN)

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

	TODO: 	Command to show protocol hierarchy like wireshark

			proto_chain: empty;
			dissect(..., proto_chain, ...) --> Recursive population of proto_chain;
			...
			proto_chain: [{ proto_num, name, bytes }, { proto_num, name, bytes }, ..., { proto_num, name, bytes }];
			...
			Worker --> Update hierachy based on proto_chain
*/

static void _deallocate_heap(
	raw_array *packets,
	shared_libs *libs,
	custom_dissectors *custom_diss,
	hierarchy_node *root
);

int main(void) {
	char *line = NULL;
	size_t len = 0;
	cmd_retval retval = RET_NONE;
	raw_array packets = { 0 };
	shared_libs libs = { 0 };
	custom_dissectors custom_diss = { 0 };
	command cmd = { 0 };
	hierarchy_node *root = create_hierarchy_node(DEFAULT_HIERARCHY_NODE_NAME, 0, 0, 0);

	load_proto_hashmaps();

	while (NULL != (line = easycli(PROMPT_STRING, E_DEFAULT_MAX_INPUT_LEN))) {
		retval = RET_NONE;
		len = (NULL != line) ? strlen(line) : 0;

		if (0 == create_cmd_from_str(&cmd, line, len))
			retval = execute_command(&cmd, &packets, &libs, &custom_diss, root);

		if (RET_UNKNOWN == retval)
			raise_error(UNKNOWN_COMMAND_ERROR, 0, UNKNOWN_COMMAND_HINT, cmd.label);
		else if (RET_EXIT == retval) {
			reset_cmd(&cmd);
			free(line);
			break;
		}
		reset_cmd(&cmd);
		free(line);
	}

	_deallocate_heap(&packets, &libs, &custom_diss, root);
	return 0;
}

static void _deallocate_heap(
	raw_array *packets,
	shared_libs *libs,
	custom_dissectors *custom_diss,
	hierarchy_node *root
) {
	reset_arr(packets, destroy_packet);
	destroy_shared_libs(libs);
	destroy_custom_dissectors(custom_diss);
	destroy_proto_hashmaps();
	destroy_hierarchy_node(root);
}