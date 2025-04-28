#include <stdio.h>

#include "ipv6.h"
#include "../ip_protos.h"
#include "../../utils/visualizer.h"
#include "../../utils/formats.h"
#include "../proto_tables_nums.h"


void print_ipv6_hdr(const uint8_t *pkt, uint32_t len) {
    uint32_t vtc_flow = IPV6_VTC_FLOW(pkt); 

    print_ipv6(IPV6_SRC_ADDR(pkt), NULL);
    printf(" > ");
    print_ipv6(IPV6_DEST_ADDR(pkt), NULL);

    printf(
        " version: %u, traffic_class: %u, flow_label: %u, payload_len: %u, next_header: %u, hop_limit: %u",
        (vtc_flow >> 28) & IPV6_VERSION,
        (vtc_flow >> 20) & IPV6_TRAFFIC_CLASS,
        vtc_flow & IPV6_FLOW,
        IPV6_PAYLOAD_LENGTH(pkt),
        IPV6_NEXT_HEADER(pkt),
        IPV6_HOP_LIMIT(pkt)
    );
}

void visualize_ipv6_hdr(const uint8_t *pkt, uint32_t len) {
    char version[3];  /* 4 bits --> max = 15 --> 16'\0' 3 chars */
    char traffic_class[4];  /* 8 bits --> max = 255 --> 255'\0' 4 chars */
    char flow_label[8];  /* 20 bits --> max = 1048575 --> 1048575'\0' 8 chars */
    char payload_len[6];  /* 16 bits --> max = 65535 --> 65535'\0' 6 chars */
    char next_header[4];  /* 8 bits --> max = 255 --> 255'\0' 4 chars */
    char hop_limit[4];
    char src_addr[IPV6_ADDR_STR_LEN];
    char dest_addr[IPV6_ADDR_STR_LEN];
    uint32_t vtc_flow = IPV6_VTC_FLOW(pkt); 

    snprintf(version, sizeof(version), "%u", (vtc_flow >> 28) & IPV6_VERSION);
    snprintf(traffic_class, sizeof(traffic_class), "%u", (vtc_flow >> 20) & IPV6_TRAFFIC_CLASS);
    snprintf(flow_label, sizeof(flow_label), "%u", vtc_flow & IPV6_FLOW);
    snprintf(payload_len, sizeof(payload_len), "%u", IPV6_PAYLOAD_LENGTH(pkt));
    snprintf(next_header, sizeof(next_header), "%u", IPV6_NEXT_HEADER(pkt));
    snprintf(hop_limit, sizeof(hop_limit), "%u", IPV6_HOP_LIMIT(pkt));
    print_ipv6(IPV6_SRC_ADDR(pkt), src_addr);
    print_ipv6(IPV6_DEST_ADDR(pkt), dest_addr);

    start_printing();
    print_field(IPV6_VERSION_LABEL, version, 0);
    print_field(IPV6_TRAFFIC_CLASS_LABEL, traffic_class, 0);
    print_field(IPV6_FLOW_LABEL, flow_label, 0);
    print_field(IPV6_PAYLOAD_LENGTH_LABEL, payload_len, 0);
    print_field(IPV6_NEXT_HEADER_LABEL, next_header, 0);
    print_field(IPV6_HOP_LIMIT_LABEL, hop_limit, 0);
    print_field(IPV6_SRC_ADDR_LABEL, src_addr, 0);
    print_field(IPV6_DEST_ADDR_LABEL, dest_addr, 0);
    end_printing();
}

protocol_info dissect_ipv6(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_ipv6_hdr, visualize_ipv6_hdr);
    return (protocol_info){ .protocol = IPV6_NEXT_HEADER(pkt), .offset = IPV6_HEADER_LEN, .proto_table_num = IP_PROTOS };
}