#include <stdio.h>

#include "icmpv6.h"


void print_icmpv6_hdr(const uint8_t *pkt) {

}

void visualize_icmpv6_hdr(const uint8_t *pkt) {

}

protocol_info dissect_icmpv6(const uint8_t *pkt, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_icmpv6_hdr, visualize_icmpv6_hdr);
    return NO_ENCAP_PROTO;
}