#include <string.h>

#include "protocol_handler.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/timestamp.h"
#include "utils/colors.h"
#include "protocols/dlt_protos.h"
#include "protocols/proto_tables_nums.h"

#define INLINE_SEPARATOR 		" | "
#define SPACE_SEPARATOR 		"\n"
#define UNKNOWN_PROTO_LABEL 	"Unknown"


output_format get_output_format(command *cmd) {
	char *out_format = get_raw_val(cmd, OUTPUT_FORMAT_ARG);
	if (NULL == out_format) return OUTPUT_FORMAT_BASIC;

	if (is_command(cmd, ANALIZE_COMMAND)) return OUTPUT_FORMAT_BASIC;
	else {
		if (0 == strcmp(out_format, OUTPUT_ARG_VAL_STD)) return OUTPUT_FORMAT_BASIC;
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_RAW)) return OUTPUT_FORMAT_RAW;
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_ART)) return OUTPUT_FORMAT_ACII_ART;
	}
	return OUTPUT_FORMAT_BASIC;
}

void print_separator(command *cmd) {
	char *out_format = get_raw_val(cmd, OUTPUT_FORMAT_ARG);
	if (NULL == out_format) {
		printf(INLINE_SEPARATOR);
		return;
	}

	if (is_command(cmd, ANALIZE_COMMAND)) printf(INLINE_SEPARATOR);
	else {
		if (0 == strcmp(out_format, OUTPUT_ARG_VAL_STD)) printf(INLINE_SEPARATOR);
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_RAW)) printf(INLINE_SEPARATOR);
		else if (0 == strcmp(out_format, OUTPUT_ARG_VAL_ART)) printf(SPACE_SEPARATOR);
	}
}

int should_print_pkt(command *cmd, protocol_layer layer) {
	int show_datalink = NULL != get_arg(cmd, DATALINK_HDR_ARG);
	int show_network = NULL == get_arg(cmd, NETWORK_HDR_ARG);
	int show_transport = NULL != get_arg(cmd, TRANSPORT_HDR_ARG);
	int show_application = NULL != get_arg(cmd, APPLICATION_HDR_ARG);

	switch (layer) {
		case PROTOCOL_LAYER_DATALINK:		return show_datalink; break;
		case PROTOCOL_LAYER_TRANSPORT:		return show_transport; break;
		case PROTOCOL_LAYER_NETWORK:		return show_network; break;
		case PROTOCOL_LAYER_APPLICATION:	return show_application; break;
		default: break;
	}
	return 1;
}

void dissect(
	command *cmd, 
	uint8_t *pkt, 
	size_t pkt_len, 
	int proto_id, 
	int proto_table_num,
	shared_libs *libs,
	custom_dissectors *custom_diss,
	int proto_shown
) {
	protocol_handler handler;
	protocol_handler *custom_handler;
	protocol_info encap_proto_info;
	output_format out_format = OUTPUT_FORMAT_NONE;
	protocol_handler *proto_hashmap = get_proto_table(proto_table_num);

	custom_handler = get_custom_protocol_handler(custom_diss, proto_id, proto_hashmap, libs);
	
	if (NULL != custom_handler) handler = *custom_handler;
	else handler = get_protocol_handler(proto_id, proto_hashmap);

	if (NULL == handler.dissect_proto) {
		if (OUTPUT_FORMAT_RAW != get_output_format(cmd)) return;
		if (pkt_len > 0) {
			if (proto_shown > 0) print_separator(cmd);
			printf(CYAN "(%s) " RESET_COLOR, UNKNOWN_PROTO_LABEL);
			SHOW_OUTPUT(pkt, pkt_len, get_output_format(cmd), NULL, NULL);
			proto_shown ++;
		}
		return;
	}
	
	if (should_print_pkt(cmd, handler.layer)) {  /* else -> default output format is OUTPUT_NONE */
		out_format = get_output_format(cmd);
		if (proto_shown > 0) print_separator(cmd);
		proto_shown ++;
		
		if (NULL == get_arg(cmd, NO_PROTOCOL_NAME_ARG)) {
			printf(CYAN "(%s) " RESET_COLOR, handler.protocol_name);
			if (OUTPUT_FORMAT_ACII_ART == out_format) printf("\n");  /* (PROTO_NAME) \n "Actual ASCII art" */
		}
	}

	/* if NO_PROTOCOL_NAME_ARG not inserted, than set the proto_name to the actual protocol name */
	encap_proto_info = handler.dissect_proto(pkt, pkt_len, out_format);
	if (-1 != encap_proto_info.proto_table_num && pkt_len > encap_proto_info.offset) {
		dissect(
			cmd,
			(pkt + encap_proto_info.offset),
			(pkt_len - encap_proto_info.offset),
			encap_proto_info.protocol,
			encap_proto_info.proto_table_num,
			libs,
			custom_diss,
			proto_shown
		);
	}
}

void dissect_packet(command *cmd, packet *pkt, shared_libs *libs, custom_dissectors *custom_diss) {
	if (NULL == get_arg(cmd, NO_TIMESTAMP_ARG)) print_timestamp(pkt->header->ts);
	if (NULL != get_arg(cmd, PACKET_NUM_ARG)) printf(GREEN "(#%ld) " RESET_COLOR, pkt->num);
	if (OUTPUT_FORMAT_ACII_ART == get_output_format(cmd)) printf("\n\n");  /* if "ascii_art" than it adds a bit of spacing */

	dissect(cmd, pkt->bytes, (size_t)pkt->header->caplen, pkt->datalink_type, DLT_PROTOS, libs, custom_diss, 0);
	printf("\n");
}