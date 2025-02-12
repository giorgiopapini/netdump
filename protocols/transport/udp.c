#include <stdio.h>
#include <arpa/inet.h>

#include "udp.h"
#include "../net_ports.h"
#include "../../utils/visualizer.h"


void print_udp_hdr(const uint8_t *pkt, uint32_t len) {
    printf(
        "src_port: %u, dest_port: %u, length: %u, cksum: 0x%04x", 
        ntohs(UDP_SRC_PORT(pkt)),
        ntohs(UDP_DEST_PORT(pkt)),
        ntohs(UDP_LENGTH(pkt)),
        ntohs(UDP_CHECKSUM(pkt))
    );
}

void visualize_udp_hdr(const uint8_t *pkt, uint32_t len) {
    char src_port[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    char dest_port[6];
    char length[6];
    char checksum[7];  /* 0x0000'\0' */

    snprintf(src_port, sizeof(src_port), "%u", ntohs(UDP_SRC_PORT(pkt)));
    snprintf(dest_port, sizeof(dest_port), "%u", ntohs(UDP_DEST_PORT(pkt)));
    snprintf(length, sizeof(length), "%u", ntohs(UDP_LENGTH(pkt)));
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(UDP_CHECKSUM(pkt)));
    
    start_printing();
    print_hdr_info(UDP_HEADER_LABEL, NULL);
    print_field(UDP_SRC_PORT_LABEL, src_port, 0);
    print_field(UDP_DEST_PORT_LABEL, dest_port, 0);
    print_field(UDP_LENGTH_LABEL, length, 0);
    print_field(UDP_CHECKSUM_LABEL, checksum, 0);
    end_printing();
}

protocol_info dissect_udp(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    protocol_info proto_info;
    proto_info.offset = UDP_LEN;
    proto_info.table = net_ports;

    SHOW_OUTPUT(pkt, pkt_len, fmt, proto_name, print_udp_hdr, visualize_udp_hdr);

    if (IS_WELL_DEFINED_PORT(ntohs(UDP_DEST_PORT(pkt)))) proto_info.protocol = ntohs(UDP_DEST_PORT(pkt));
    else if (IS_WELL_DEFINED_PORT(ntohs(UDP_SRC_PORT(pkt)))) proto_info.protocol = ntohs(UDP_SRC_PORT(pkt));
    else proto_info = NO_ENCAP_PROTO;

    return proto_info;
}