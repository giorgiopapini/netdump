#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../status_handler.h"
#include "colors.h"


static void _print_ascii_raw_pkt(const uint8_t *pkt, size_t len);
static void _print_raw_pkt(const uint8_t *pkt, size_t len);


static void _print_ascii_raw_pkt(const uint8_t *pkt, size_t len) {
	size_t i;

	for (i = 0; i < len; i ++) {
		if (isprint(pkt[i])) printf("%c", pkt[i]);
		else printf(".");
	}
}

static void _print_raw_pkt(const uint8_t *pkt, size_t len) {
	const uint8_t *tmp_pkt = pkt;
	size_t line_chars = 4 * 8 + 7;
	size_t padding;
	size_t hex_chars;
	size_t whitespaces;
	size_t i, j;

	CHECK_NULL_RET(pkt);
	for (i = 0; i < len; i ++) {
		if (0 == i % 16) {
			tmp_pkt = &pkt[i];
			printf(YELLOW "\n0x%04lx: " RESET_COLOR, i);
		}
		printf("%02x", pkt[i]);
		fflush(stdout);

		if (0 == (i + 1) % 2) {
			if (0 == (i + 1) % 16) {
				printf(" | ");
				_print_ascii_raw_pkt(tmp_pkt, 16);
			}
			else if (i < len - 1) printf(" ");
			else {
				hex_chars = (len % 16) * 2;
				whitespaces = (hex_chars / 4) - 1;
				padding = line_chars - hex_chars - whitespaces;
				for (j = 0; j < padding; j ++) printf(" ");
				printf(" | ");
				_print_ascii_raw_pkt(tmp_pkt, (len % 16));
			}
		}
	}
	printf("\n");
}

output_func_t select_output_func(
	output_format fmt,
	output_func_t print_func,
	output_func_t visualize_func
) {
	switch (fmt) {
		case OUTPUT_FORMAT_NONE: 		return NULL;
		case OUTPUT_FORMAT_RAW:			return _print_raw_pkt;
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
	protocol_handler *new_handler = malloc(sizeof *new_handler);
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
	protocol_handler_mapping *new_mapping = malloc(sizeof *new_mapping);
	CHECK_NULL_EXIT(new_mapping);

	new_mapping->handler = handler;
	new_mapping->proto_table_num = proto_table_num;
	return new_mapping;
}

protocol_handler_mapping **create_mappings_arr(void) {  /* NULL terminated array */
	protocol_handler_mapping **new_mappings = malloc(2 * sizeof *new_mappings);
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

protocol_handler *get_protocol_handler(int target_proto, hashmap *proto_table) {
	hashmap_entry *entry = get_entry(proto_table, target_proto);
	if (NULL == entry) return NULL;
	
	return (protocol_handler *)entry->value;
}