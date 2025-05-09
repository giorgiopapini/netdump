#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>


typedef enum output_format {
	OUTPUT_FORMAT_NONE = -1,
	OUTPUT_FORMAT_BASIC,
	OUTPUT_FORMAT_RAW,
	OUTPUT_FORMAT_ACII_ART,
} output_format;

typedef enum protocol_layer {
	PROTOCOL_LAYER_NONE = -1,
	PROTOCOL_LAYER_DATALINK,
	PROTOCOL_LAYER_NETWORK,
	PROTOCOL_LAYER_TRANSPORT,
	PROTOCOL_LAYER_APPLICATION
} protocol_layer;

typedef struct protocol_info {
    int protocol;
    size_t offset;
    int proto_table_num;
} protocol_info;

typedef struct protocol_handler {
    int protocol;
	protocol_layer layer;
    protocol_info (*dissect_proto)(const uint8_t *pkt, size_t pkt_len, output_format fmt);
    const char *protocol_name;
} protocol_handler;

typedef struct protocol_handler_mapping {
	protocol_handler *handler;
	int proto_table_num;
} protocol_handler_mapping;

typedef void (*output_func_t)(const uint8_t *, size_t);


#define NO_ENCAP_PROTO			(protocol_info){ .protocol = -1, .offset = 0, .proto_table_num = -1 };
#define NULL_PROTO_HANDLER		{ .protocol = 0, .layer = PROTOCOL_LAYER_NONE, .dissect_proto = NULL, .protocol_name = NULL }
#define IS_NULL_HANDLER(hdl)	(0 == hdl.protocol && PROTOCOL_LAYER_NONE == hdl.layer && NULL == hdl.dissect_proto && NULL == hdl.protocol_name)
#define SHOW_OUTPUT(pkt, len, fmt, print_func, visualize_func) \
		do { \
			output_func_t output_func = select_output_func(fmt, print_func, visualize_func); \
    		if (NULL != output_func && len > 0) output_func(pkt, len); \
		} while(0)

output_func_t select_output_func(
    output_format fmt,
    void (*print_func)(const uint8_t *, size_t), 
    void (*visualize_func)(const uint8_t *, size_t)
);
protocol_handler *create_protocol_handler(
	int proto, 
	protocol_layer layer, 
	protocol_info (*dissect_proto)(const uint8_t *pkt, size_t pkt_len, output_format fmt),
	const char *protocol_name
);
protocol_handler_mapping *create_protocol_handler_mapping(
	protocol_handler *handler,
	int proto_table_num
);
protocol_handler_mapping **create_mappings_arr(void);
void add_mapping(protocol_handler_mapping ***arr_ptr, protocol_handler_mapping *new_mapping);
void destroy_mappings(protocol_handler_mapping **mappings);
protocol_handler get_protocol_handler(int target_proto, protocol_handler *proto_table);

#endif