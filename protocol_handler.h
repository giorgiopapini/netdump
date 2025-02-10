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
    protocol_info (*dissect_proto)(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt);
    const char *protocol_name;
} protocol_handler;

#define NO_ENCAP_PROTO		(protocol_info){ .protocol = -1, .offset = 0, .table = NULL };
#define SHOW_OUTPUT(pkt, fmt, proto_name, print_func, visualize_func) \
		do { \
			switch(fmt) { \
				case OUTPUT_FORMAT_NONE:		break; \
				case OUTPUT_FORMAT_BASIC: { \
					if (NULL != proto_name) printf(CYAN "(%s) " RESET_COLOR, proto_name); \
					print_func(pkt); \
					break; \
				} case OUTPUT_FORMAT_ACII_ART:	visualize_func(pkt); break; \
				default: break; \
			} \
		} while(0)

protocol_handler get_protocol_handler(int target_proto, protocol_handler *proto_table);
void dissect_packet(command *cmd, packet *pkt);

#endif