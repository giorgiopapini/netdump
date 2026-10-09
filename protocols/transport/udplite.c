#include "udplite.h"

#include <stdio.h>

#include "../net_ports.h"
#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static void _print_udplite_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_udplite_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _print_udplite_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    uint16_t coverage;

    if (!pkt || pkt_len < hdr_len) return;

    coverage = UDPLITE_CHECKSUM_COVERAGE(pkt);

    printf(
        "src_port: %u, dest_port: %u, checksum_coverage: %u, cksum: 0x%04x",
        UDPLITE_SRC_PORT(pkt),
        UDPLITE_DEST_PORT(pkt),
        coverage,
        UDPLITE_CHECKSUM(pkt)
    );

    if (0 == coverage) printf(" (covers the entire datagram)");
    else if (coverage < UDPLITE_HDR_LEN) printf(", note: coverage must be 0 or >= %d", UDPLITE_HDR_LEN);
    else if ((size_t)coverage > pkt_len) printf(", note: coverage exceeds the %lu-byte datagram", (unsigned long)pkt_len);
    else printf(" (header + %u payload byte(s))", (unsigned)(coverage - UDPLITE_HDR_LEN));
}

static void _visualize_udplite_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char src_port[6];   /* 16 bit ==> max = 65535 (5 chars + '\0') */
    char dest_port[6];
    char coverage[6];
    char checksum[7];   /* 0x0000'\0' */

    if (!pkt || pkt_len < hdr_len) return;

    snprintf(src_port, sizeof(src_port), "%u", UDPLITE_SRC_PORT(pkt));
    snprintf(dest_port, sizeof(dest_port), "%u", UDPLITE_DEST_PORT(pkt));
    snprintf(coverage, sizeof(coverage), "%u", UDPLITE_CHECKSUM_COVERAGE(pkt));
    snprintf(checksum, sizeof(checksum), "0x%04x", UDPLITE_CHECKSUM(pkt));

    start_printing();
    print_field(UDPLITE_SRC_PORT_LABEL, src_port, 0);
    print_field(UDPLITE_DEST_PORT_LABEL, dest_port, 0);
    print_field(UDPLITE_CHECKSUM_COVERAGE_LABEL, coverage, 0);
    print_field(UDPLITE_CHECKSUM_LABEL, checksum, 0);
    end_printing();
}

protocol_info dissect_udplite(const uint8_t *pkt, size_t pkt_len) {
    protocol_info proto_info;
    if (!pkt || pkt_len < UDPLITE_HDR_LEN) return NO_PROTO_INFO;

    proto_info.hdr_len = UDPLITE_HDR_LEN;
    proto_info.encap_proto_table_num = NET_PORTS;

    /* UDP-Lite shares the UDP port number space */
    if (IS_WELL_DEFINED_PORT(UDPLITE_DEST_PORT(pkt))) proto_info.encap_protocol = UDPLITE_DEST_PORT(pkt);
    else if (IS_WELL_DEFINED_PORT(UDPLITE_SRC_PORT(pkt))) proto_info.encap_protocol = UDPLITE_SRC_PORT(pkt);
    else proto_info = NO_PROTO_INFO;

    proto_info.print_protocol_func = _print_udplite_hdr;
    proto_info.visualize_protocol_func = _visualize_udplite_hdr;

    return proto_info;
}
