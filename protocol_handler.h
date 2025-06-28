#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include <stddef.h>
#include <stdint.h>

#include "libs/libnetdump/protocol.h"
#include "utils/command.h"
#include "utils/packet.h"
#include "utils/shared_lib.h"
#include "utils/raw_array.h"
#include "utils/custom_dissectors.h"
#include "utils/hashmap.h"
#include "utils/hierachy.h"

#define DEFAULT_PROTO_CHAIN_DEPTH		5

typedef struct proto_chain_node {
	int proto_num;
	char *proto_name;
	size_t bytes;  /* hdr or "payload" if packet has no hdr like TCP */
} proto_chain_node;


proto_chain_node *create_proto_chain_node(int proto_num, const char *proto_name, size_t bytes);
void destroy_proto_chain_node(void *node);  /* void pointer because this function will be called inside reset_arr() function */
output_format get_output_format(command *cmd);
protocol_handler *get_protocol_handler(int target_proto, hashmap *proto_table);
void print_separator(command *cmd);
int should_print_pkt(command *cmd, protocol_layer layer);
void dissect_packet(command *cmd, packet *pkt, shared_libs *libs, custom_dissectors *custosm_diss, hierarchy_node *root);
void dissect(
	command *cmd,
	raw_array *proto_chain,
	uint8_t *pkt, 
	size_t pkt_len, 
	int proto_id, 
	int proto_table_num,
	shared_libs *libs,
	custom_dissectors *custom_diss,
	int proto_shown
);

#endif