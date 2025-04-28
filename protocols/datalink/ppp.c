#include <stdio.h>

#include "ppp.h"
#include "../../utils/visualizer.h"
#include "../proto_tables_nums.h"


void print_ppp_hdr(const uint8_t *pkt, uint32_t len) {
    printf(
        "addr: 0x%02x, control: 0x%02x, protocol: 0x%04x",
        ADDRESS(pkt),
        CONTROL(pkt),
        PROTOCOL(pkt)
    );
}

void visualize_ppp_hdr(const uint8_t *pkt, uint32_t len) {
    char address[5];  /* 0x00'\0' are 5 chars */
    char control[5];
    char protocol[7];  /* 0x0000'\0' are 7 chars */

    snprintf(address, sizeof(address), "0x%02x", ADDRESS(pkt));
    snprintf(control, sizeof(control), "0x%02x", CONTROL(pkt));
    snprintf(protocol, sizeof(protocol), "0x%04x", PROTOCOL(pkt));

    start_printing();
    print_field(ADDRESS_LABEL, address, 0);
    print_field(CONTROL_LABEL, control, 0);
    print_field(PROTOCOL_LABEL, protocol, 0);
    end_printing();
}

protocol_info dissect_ppp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_ppp_hdr, visualize_ppp_hdr);
    return (protocol_info){ .protocol = PROTOCOL(pkt), .offset = PPP_LEN, .proto_table_num = PPP_PROTOS };
}