#include "ether.h"

#include <stdio.h>

#include "../../utils/formats.h"
#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static void _print_ether_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_ether_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _print_ether_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    if (!pkt || pkt_len < hdr_len) return;
    /* ========================= printing src (MAC) > dest (MAC) ========================= */
    print_mac(SRC_ADDR(pkt));
    printf(" > ");
	print_mac(DEST_ADDR(pkt));
    /* =================================================================================== */

	/* =============================== printing ethertype ================================ */
	printf(", ethertype: 0x%04x", ETHERTYPE(pkt)); 
    /* =================================================================================== */
}

static void _visualize_ether_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char dest_addr[MAC_ADDR_STR_LEN];
    char src_addr[MAC_ADDR_STR_LEN];
    char ethertype[7];  /* 0xXXXX'\0' */

    if (!pkt || pkt_len < hdr_len) return;
    
    snprintf(dest_addr, sizeof(dest_addr), MAC_ADDR_FORMAT, MAC_TO_STR(DEST_ADDR(pkt)));
    snprintf(src_addr, sizeof(src_addr), MAC_ADDR_FORMAT, MAC_TO_STR(SRC_ADDR(pkt)));
    snprintf(ethertype, sizeof(ethertype), "0x%04x", ETHERTYPE(pkt));

    start_printing();
    print_field(DEST_ADDR_LABEL, dest_addr, 0);
    print_field(SRC_ADDR_LABEL, src_addr, 0);
    print_field(ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}

protocol_info dissect_ether(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < ETHER_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_ether_hdr,
        .visualize_protocol_func = _visualize_ether_hdr,
        .hdr_len = ETHER_HDR_LEN,
        .encap_protocol = ETHERTYPE(pkt),
        .encap_proto_table_num = ETHERTYPES
    };
}