#include <stdio.h>

#include "protocol.h"


void print_raw_pkt(const uint8_t *pkt, uint32_t len) {
	int i;
	if (len > 0) printf("[%02x", *pkt);
	for (i = 1; i < len; i ++) printf(" %02x", pkt[i]);
	printf("]");
}

void *select_output_func(
	output_format fmt,
	void (*print_func)(const uint8_t *, uint32_t), 
	void (*visualize_func)(const uint8_t *, uint32_t)
) {
	switch (fmt) {
		case OUTPUT_FORMAT_NONE: 		return NULL;
		case OUTPUT_FORMAT_RAW:			return print_raw_pkt;
		case OUTPUT_FORMAT_BASIC: 		return print_func;
		case OUTPUT_FORMAT_ACII_ART:	return visualize_func;
		default:						return NULL;
	}
}

protocol_handler get_protocol_handler(int target_proto, protocol_handler *proto_table) {
	protocol_handler *proto_handler = NULL;
	int i;

	for (i = 0; !IS_NULL_HANDLER(proto_table[i]); i ++) {
		if (target_proto == proto_table[i].protocol) return proto_table[i];
	}
	return (protocol_handler){ 0, PROTOCOL_LAYER_NONE, NULL, NULL };
}