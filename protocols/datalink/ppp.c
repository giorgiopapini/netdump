#include "ppp.h"

#include <stdio.h>

#include "../../utils/visualizer.h"
#include "../proto_tables_handler.h"
#include "../../utils/protocol.h"


static void _print_ppp_hdr(const uint8_t *pkt, size_t pkt_len);
static void _visualize_ppp_hdr(const uint8_t *pkt, size_t pkt_len);

static void _print_ppp_hdr(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < PPP_HDR_LEN) return;
    
    printf(
        "addr: 0x%02x, control: 0x%02x, protocol: 0x%04x",
        ADDRESS(pkt),
        CONTROL(pkt),
        PROTOCOL(pkt)
    );
}

static void _visualize_ppp_hdr(const uint8_t *pkt, size_t pkt_len) {
    char address[5];  /* 0x00'\0' are 5 chars */
    char control[5];
    char protocol[7];  /* 0x0000'\0' are 7 chars */
    
    if (!pkt || pkt_len < PPP_HDR_LEN) return;
    
    snprintf(address, sizeof(address), "0x%02x", ADDRESS(pkt));
    snprintf(control, sizeof(control), "0x%02x", CONTROL(pkt));
    snprintf(protocol, sizeof(protocol), "0x%04x", PROTOCOL(pkt));

    start_printing();
    print_field(ADDRESS_LABEL, address, 0);
    print_field(CONTROL_LABEL, control, 0);
    print_field(PROTOCOL_LABEL, protocol, 0);
    end_printing();
}

protocol_info dissect_ppp(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    if (!pkt || pkt_len < PPP_HDR_LEN) return NO_ENCAP_PROTO;

    SHOW_OUTPUT(pkt, pkt_len, fmt, _print_ppp_hdr, _visualize_ppp_hdr);
    return (protocol_info){ .protocol = PROTOCOL(pkt), .offset = PPP_HDR_LEN, .proto_table_num = PPP_PROTOS };
}