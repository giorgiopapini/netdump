#include <stdio.h>

#include "udp.h"
#include "../net_ports.h"
#include "../../utils/visualizer.h"
#include "../proto_tables_nums.h"


void print_udp_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_udp_hdr(const uint8_t *pkt, size_t pkt_len);

void print_udp_hdr(const uint8_t *pkt, size_t pkt_len) {
    (void)pkt_len;

    printf(
        "src_port: %u, dest_port: %u, length: %u, cksum: 0x%04x", 
        UDP_SRC_PORT(pkt),
        UDP_DEST_PORT(pkt),
        UDP_LENGTH(pkt),
        UDP_CHECKSUM(pkt)
    );
}

void visualize_udp_hdr(const uint8_t *pkt, size_t pkt_len) {
    char src_port[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    char dest_port[6];
    char length[6];
    char checksum[7];  /* 0x0000'\0' */
    (void)pkt_len;

    snprintf(src_port, sizeof(src_port), "%u", UDP_SRC_PORT(pkt));
    snprintf(dest_port, sizeof(dest_port), "%u", UDP_DEST_PORT(pkt));
    snprintf(length, sizeof(length), "%u", UDP_LENGTH(pkt));
    snprintf(checksum, sizeof(checksum), "0x%04x", UDP_CHECKSUM(pkt));
    
    start_printing();
    print_field(UDP_SRC_PORT_LABEL, src_port, 0);
    print_field(UDP_DEST_PORT_LABEL, dest_port, 0);
    print_field(UDP_LENGTH_LABEL, length, 0);
    print_field(UDP_CHECKSUM_LABEL, checksum, 0);
    end_printing();
}

protocol_info dissect_udp(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    protocol_info proto_info;
    proto_info.offset = UDP_LEN;
    proto_info.proto_table_num = NET_PORTS;

    SHOW_OUTPUT(pkt, pkt_len, fmt, print_udp_hdr, visualize_udp_hdr);

    if (IS_WELL_DEFINED_PORT(UDP_DEST_PORT(pkt))) proto_info.protocol = UDP_DEST_PORT(pkt);
    else if (IS_WELL_DEFINED_PORT(UDP_SRC_PORT(pkt))) proto_info.protocol = UDP_SRC_PORT(pkt);
    else proto_info = NO_ENCAP_PROTO;

    return proto_info;
}