#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>

#include "../status_handler.h"


void print_raw_pkt(const uint8_t *pkt, size_t len);

void print_raw_pkt(const uint8_t *pkt, size_t len) {
	size_t i;

	CHECK_NULL_RET(pkt);
	if (len > 0) printf("[%02x", *pkt);
	for (i = 1; i < len; i ++) printf(" %02x", pkt[i]);
	printf("]");
}

output_func_t select_output_func(
	output_format fmt,
	output_func_t print_func,
	output_func_t visualize_func
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
	protocol_info (*dissect_proto)(const uint8_t *pkt, size_t pkt_len, output_format fmt),
	const char *protocol_name
) {
	protocol_handler *new_handler = (protocol_handler *)malloc(sizeof(protocol_handler));
	CHECK_NULL_EXIT(new_handler);

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
	CHECK_NULL_EXIT(new_mapping);

	new_mapping->handler = handler;
	new_mapping->proto_table_num = proto_table_num;
	return new_mapping;
}

protocol_handler_mapping **create_mappings_arr(void) {  /* NULL terminated array */
	protocol_handler_mapping **new_mappings = (protocol_handler_mapping **)malloc(sizeof(protocol_handler_mapping *) * 2);
	CHECK_NULL_EXIT(new_mappings);

    new_mappings[0] = NULL;
    new_mappings[1] = NULL;
    return new_mappings;
}

void add_mapping(protocol_handler_mapping ***arr_ptr, protocol_handler_mapping *new_mapping) {
	int count = 0;

	CHECK_NULL_EXIT(arr_ptr);
	while (*arr_ptr && (*arr_ptr)[count] != NULL) count ++;

	*arr_ptr = (protocol_handler_mapping **)realloc(*arr_ptr, (size_t)(count + 2) * sizeof(protocol_handler_mapping *));
	CHECK_NULL_EXIT(*arr_ptr);

    (*arr_ptr)[count] = new_mapping;
    (*arr_ptr)[count + 1] = NULL;
}

void destroy_mappings(protocol_handler_mapping **mappings) {
	size_t i;

	if (NULL != mappings) {
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
	size_t i;

	if (NULL != proto_table) {
		for (i = 0; !IS_NULL_HANDLER(proto_table[i]); i ++) {
			if (target_proto == proto_table[i].protocol) return proto_table[i];
		}
	}
	return (protocol_handler){ 0, PROTOCOL_LAYER_NONE, NULL, NULL };
}