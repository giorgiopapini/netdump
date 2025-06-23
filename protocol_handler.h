#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include <stddef.h>
#include <stdint.h>

#include "utils/command.h"
#include "utils/packet.h"
#include "utils/shared_lib.h"
#include "utils/protocol.h"
#include "utils/custom_dissectors.h"
#include "utils/hashmap.h"


output_format get_output_format(command *cmd);
protocol_handler *get_protocol_handler(int target_proto, hashmap *proto_table);
void print_separator(command *cmd);
int should_print_pkt(command *cmd, protocol_layer layer);
void dissect_packet(command *cmd, packet *pkt, shared_libs *libs, custom_dissectors *custosm_diss);
void dissect(
	command *cmd, 
	uint8_t *pkt, 
	size_t pkt_len, 
	int proto_id, 
	int proto_table_num,
	shared_libs *libs,
	custom_dissectors *custom_diss,
	int proto_shown
);

#endif