#include "protocol_handler.h"
#include "../status_handler.h"
#include "datalink_handler.h"
#include "network_handler.h"
#include "../utils/timestamp.h"

#include <arpa/inet.h>

#define PRINT_SEPARATOR " | "


void exec_behaviour(command *cmd, protocol_info info, uint8_t *bytes) {
	if (is_command(cmd, ANALIZE_COMMAND) && NULL != info.print_header) info.print_header(bytes);
	if (is_command(cmd, PRINT_COMMAND) && NULL != info.print_header) info.print_header(bytes);
	if (is_command(cmd, VISUALIZE_COMMAND) && NULL != info.visualize_header) info.visualize_header(bytes);
}

int datalink_behaviour(command *cmd, protocol_info info, uint8_t *bytes) {
	int show_datalink = NULL != get_arg(cmd, DATALINK_HDR_ARG);

	if (show_datalink) exec_behaviour(cmd, info, bytes);
	return show_datalink;
}

int network_behaviour(command *cmd, protocol_info info, uint8_t *bytes) {
	int show_network = NULL == get_arg(cmd, NETWORK_HDR_ARG);

	if (show_network) exec_behaviour(cmd, info, bytes);
	return show_network;
}

void dissect_packet(command *cmd, packet *pkt) {
	uint8_t *raw_pkt = pkt->bytes;
    int show_datalink;
	int show_network;
	int net_protocol_type = 0;
	int trans_protocol_type = 0;

	if (NULL == get_arg(cmd, NO_TIMESTAMP_ARG)) print_timestamp(pkt->header->ts);
	if (NULL != get_arg(cmd, PACKET_NUM_ARG)) printf("(#%d) ", pkt->num);

	/* =========================== dissect datalink =========================== */
	protocol_info datalink_info = dissect_datalink(pkt->datalink_type);
	show_datalink = datalink_behaviour(cmd, datalink_info, raw_pkt);
	net_protocol_type = get_field(raw_pkt, datalink_info.encap_type_range);
	/* ======================================================================== */
	
	/* =========================== dissect network ============================ */
	raw_pkt += datalink_info.hdr_size;
	protocol_info network_info = dissect_network(net_protocol_type);
	show_network = network_behaviour(cmd, network_info, raw_pkt);
	trans_protocol_type = get_field(raw_pkt, network_info.encap_type_range);
	/* ======================================================================== */

	/* ========================== dissect transport =========================== */
	raw_pkt += network_info.hdr_size;
	/* ======================================================================== */
	printf("\n");
}

int get_field(const uint8_t *pkt, field byte_segment) {
    int i;
    int val = 0;

    if (NULL == pkt) raise_error(NULL_POINTER, 1, NULL, "pkt", __FILE__);

    if (1 == byte_segment.len) return pkt[byte_segment.start];
    for (i = 0; i < byte_segment.len; i ++) {
        val |= pkt[byte_segment.start + i] << (8 * i);
    }

    if (0 != byte_segment.ntoh) {
        if (2 == byte_segment.len) return ntohs(val);
        if (4 == byte_segment.len) return ntohl(val);
    }
    return val;
}