#include <stdio.h>
#include <arpa/inet.h>

#include "udp.h"
#include "../../utils/visualizer.h"


size_t udp_hdr_len(const uint8_t *pkt) { return UDP_LEN; }

void print_udp_hdr(const uint8_t *pkt) {
    printf(
        "src port: %u, dest port: %u, length: %u, cksum: 0x%04x", 
        ntohs(SRC_PORT(pkt)),
        ntohs(DEST_PORT(pkt)),
        ntohs(UDP_LENGTH(pkt)),
        ntohs(CHECKSUM(pkt))
    );
}

void visualize_udp_hdr(const uint8_t *pkt) {
    char src_port[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    char dest_port[6];
    char length[6];
    char checksum[7];  /* 0x0000'\0' */

    snprintf(src_port, sizeof(src_port), "%u", ntohs(SRC_PORT(pkt)));
    snprintf(dest_port, sizeof(dest_port), "%u", ntohs(DEST_PORT(pkt)));
    snprintf(length, sizeof(length), "%u", ntohs(UDP_LENGTH(pkt)));
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(CHECKSUM(pkt)));
    
    start_printing();
    print_hdr_info(UDP_HEADER_LABEL, NULL);
    print_field(SRC_PORT_LABEL, src_port, 0);
    print_field(DEST_PORT_LABEL, dest_port, 0);
    print_field(UDP_LENGTH_LABEL, length, 0);
    print_field(CHECKSUM_LABEL, checksum, 0);
    end_printing();
}

protocol_info dissect_udp(const uint8_t *pkt, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_udp_hdr, visualize_udp_hdr);
    return NO_ENCAP_PROTO;
}