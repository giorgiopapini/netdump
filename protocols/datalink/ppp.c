#include <stdio.h>
#include <arpa/inet.h>

#include "ppp.h"
#include "../../ppp_protos.h"
#include "../../utils/visualizer.h"


size_t ppp_hdr_len(const uint8_t *pkt) { return PPP_LEN; }

void print_ppp_hdr(const uint8_t *pkt) {
    printf(
        "addr: 0x%02x, control: 0x%02x, protocol: 0x%04x",
        ADDRESS(pkt),
        CONTROL(pkt),
        ntohs(PROTOCOL(pkt))
    );
}

void visualize_ppp_hdr(const uint8_t *pkt) {
    char address[5];  /* 0x00'\0' are 5 chars */
    char control[5];
    char protocol[7];  /* 0x0000'\0' are 7 chars */

    snprintf(address, sizeof(address), "0x%02x", ADDRESS(pkt));
    snprintf(control, sizeof(control), "0x%02x", CONTROL(pkt));
    snprintf(protocol, sizeof(protocol), "0x%04x", ntohs(PROTOCOL(pkt)));

    start_printing();
    print_hdr_info(PPP_HEADER_LABEL, NULL);
    print_field(ADDRESS_LABEL, address, 0);
    print_field(CONTROL_LABEL, control, 0);
    print_field(PROTOCOL_LABEL, protocol, 0);
    end_printing();
}

protocol_info dissect_ppp(const uint8_t *pkt, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_ppp_hdr, visualize_ppp_hdr);
    return (protocol_info){ .protocol = ntohs(PROTOCOL(pkt)), .offset = PPP_LEN, .hashmap = ppp_protos };
}