#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include <stdint.h>
#include <stddef.h>

#include "command_handler.h"
#include "utils/packet.h"
#include "utils/colors.h"


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

typedef struct protocol_handler protocol_handler;

typedef struct protocol_info {
    int protocol;
    size_t offset;
    protocol_handler *table;
} protocol_info;

typedef struct protocol_handler {
    int protocol;
	protocol_layer layer;
    protocol_info (*dissect_proto)(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);
    const char *protocol_name;
} protocol_handler;

#define NO_ENCAP_PROTO			(protocol_info){ .protocol = -1, .offset = 0, .table = NULL };
#define NULL_PROTO_HANDLER		{ .protocol = 0, .layer = PROTOCOL_LAYER_NONE, .dissect_proto = NULL, .protocol_name = NULL }
#define IS_NULL_HANDLER(hdl)	(0 == hdl.protocol && PROTOCOL_LAYER_NONE == hdl.layer && NULL == hdl.dissect_proto && NULL == hdl.protocol_name)
#define SHOW_OUTPUT(pkt, len, fmt, print_func, visualize_func) \
		do { \
			void (*output_func)(const uint8_t *, uint32_t) = select_output_func(fmt, print_func, visualize_func); \
    		if (NULL != output_func && len > 0) output_func(pkt, len); \
		} while(0)

protocol_handler get_protocol_handler(int target_proto, protocol_handler *proto_table);
void *select_output_func(
	output_format fmt, 
	void (*print_func)(const uint8_t *, uint32_t), 
	void (*visualize_func)(const uint8_t *, uint32_t)
);
void dissect_packet(command *cmd, packet *pkt);

#endif