#include "protocol_handler.h"
#include "status_handler.h"
#include "utils/timestamp.h"
#include "utils/colors.h"
#include "protocols/dlt_protos.h"

#define PRINT_SEPARATOR " | "
#define VISUALIZE_SEPARATOR "\n"


output_format get_output_format(command *cmd) {
	if (is_command(cmd, ANALIZE_COMMAND)) return OUTPUT_FORMAT_BASIC;
	else if (is_command(cmd, PRINT_COMMAND)) return OUTPUT_FORMAT_BASIC;
	else if (is_command(cmd, VISUALIZE_COMMAND)) return OUTPUT_FORMAT_ACII_ART;
}

void print_separator(command *cmd) {
	if (is_command(cmd, ANALIZE_COMMAND)) printf(PRINT_SEPARATOR);
	else if (is_command(cmd, PRINT_COMMAND)) printf(PRINT_SEPARATOR);
	else if (is_command(cmd, VISUALIZE_COMMAND)) printf(VISUALIZE_SEPARATOR);
}

protocol_handler get_protocol_handler(int target_proto, protocol_handler *proto_table) {
	int i;
	for (i = 0; !IS_NULL_HANDLER(proto_table[i]); i ++) {
		protocol_handler aaa = proto_table[i];
		if (target_proto == proto_table[i].protocol) return proto_table[i];
	}
	return (protocol_handler){ 0, PROTOCOL_LAYER_NONE, NULL, NULL };
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

void dissect(command *cmd, uint8_t *pkt, bpf_u_int32 pkt_len, int proto_id, protocol_handler *proto_hasmap, int proto_shown) {
	protocol_handler handler;
	protocol_info encap_proto_info;
	output_format out_format = OUTPUT_FORMAT_NONE;
	const char *proto_name = NULL;

	handler = get_protocol_handler(proto_id, proto_hasmap);
	if (NULL == handler.dissect_proto) return;

	if (should_print_pkt(cmd, handler.layer)) {  /* else -> default output format is OUTPUT_NONE */
		out_format = get_output_format(cmd);
		if (proto_shown > 0) print_separator(cmd);
		proto_shown ++;
	}

	/* if NO_PROTOCOL_NAME_ARG not inserted, than set the proto_name to the actual protocol name */
	if (NULL == get_arg(cmd, NO_PROTOCOL_NAME_ARG)) proto_name = handler.protocol_name;
	encap_proto_info = handler.dissect_proto(pkt, pkt_len, proto_name, out_format);
	if (NULL != encap_proto_info.table && (pkt_len - encap_proto_info.offset) > 0) {
		dissect(
			cmd,
			(pkt + encap_proto_info.offset),
			(pkt_len - encap_proto_info.offset),
			encap_proto_info.protocol,
			encap_proto_info.table,
			proto_shown
		);
	}
}

void dissect_packet(command *cmd, packet *pkt) {
	if (NULL == get_arg(cmd, NO_TIMESTAMP_ARG)) print_timestamp(pkt->header->ts);
	if (NULL != get_arg(cmd, PACKET_NUM_ARG)) printf(GREEN "(#%d) " RESET_COLOR, pkt->num);
	if (is_command(cmd, VISUALIZE_COMMAND)) printf("\n\n");  /* if "visualize" than it adds a bit of spacing */

	dissect(cmd, pkt->bytes, pkt->header->caplen, pkt->datalink_type, dlt_protos, 0);
	printf("\n");
}