#include "protocol_handler.h"
#include "../status_handler.h"
#include "datalink_handler.h"
#include "network_handler.h"

#include <arpa/inet.h>


void dissect_packet(command *cmd, packet *pkt) {
	uint8_t *raw_pkt = pkt->bytes;
    int show_datalink = NULL != get_arg(cmd, DATALINK_HDR_ARG);
	int show_network = NULL == get_arg(cmd, NETWORK_HDR_ARG);
	int net_protocol_type = 0;
	int trans_protocol_type = 0;

	/* =========================== dissect datalink =========================== */
	protocol_info datalink_info = dissect_datalink(pkt->datalink_type);
	if (show_datalink && datalink_info.print_header != NULL) datalink_info.print_header(raw_pkt);
	net_protocol_type = get_field(raw_pkt, datalink_info.encap_type_range);
	/* ======================================================================== */

	/* =========================== dissect network ============================ */
	raw_pkt += datalink_info.hdr_size;
	protocol_info network_info = dissect_network(net_protocol_type);
	if (show_network && network_info.print_header != NULL) {
		if (show_datalink) printf(" | "); /* if datalink not shown, than do not print separator */
		network_info.print_header(raw_pkt);
	}
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