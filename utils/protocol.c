#include <stdio.h>
#include <stdlib.h>

#include "protocol.h"
#include "../status_handler.h"


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

protocol_handler *create_protocol_handler(
	int proto, 
	protocol_layer layer, 
	protocol_info (*dissect_proto)(const uint8_t *pkt, uint32_t pkt_len, output_format fmt),
	const char *protocol_name
) {
	protocol_handler *new_handler = (protocol_handler *)malloc(sizeof(protocol_handler));
	new_handler->protocol = proto;
	new_handler->layer = layer;
	new_handler->dissect_proto = dissect_proto;
	new_handler->protocol_name = protocol_name;
	return new_handler;
}

protocol_handler_mapping *create_protocol_handler_mapping(
	protocol_handler *handler,
	int proto_table_num
) {
	protocol_handler_mapping *new_mapping = (protocol_handler_mapping *)malloc(sizeof(protocol_handler_mapping));
	new_mapping->handler = handler;
	new_mapping->proto_table_num = proto_table_num;
	return new_mapping;
}

protocol_handler_mapping **create_mappings_arr() {  /* NULL terminated array */
	protocol_handler_mapping **new_mappings = (protocol_handler_mapping **)malloc(sizeof(protocol_handler_mapping *) * 2);
    if (!new_mappings) raise_error(NULL_POINTER, 1, NULL, "**new_mappings", __FILE__);

    new_mappings[0] = NULL;
    new_mappings[1] = NULL;
    return new_mappings;
}

void add_mapping(protocol_handler_mapping ***arr_ptr, protocol_handler_mapping *new_mapping) {
	int count = 0;
	while (*arr_ptr && (*arr_ptr)[count] != NULL) count ++;

	*arr_ptr = (protocol_handler_mapping **)realloc(*arr_ptr, (count + 2) * sizeof(protocol_handler_mapping *));
    if (!*arr_ptr) raise_error(NULL_POINTER, 1, NULL, "*arr_ptr", __FILE__);

    (*arr_ptr)[count] = new_mapping;
    (*arr_ptr)[count + 1] = NULL;
}

void destroy_mappings(protocol_handler_mapping **mappings) {
	int i;

	if (mappings) {
        for (i = 0; mappings[i] != NULL; i ++) {
            /* 
				free(mappings[i]->handler); Im not doing this because the actual handler is stored inside custom_dissectors 
				once loaded. So it should not be freed
			*/
            free(mappings[i]);
        }
        free(mappings);
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