#include "ripng.h"

#include <stdio.h>

#include "../../utils/formats.h"
#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static const char *_ripng_command_name(uint8_t command);
static int _ripng_prefix_is_multicast(const uint8_t *prefix);
static int _ripng_prefix_is_link_local(const uint8_t *prefix);
static int _ripng_prefix_is_unspecified(const uint8_t *prefix);
static void _print_ripng_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_ripng_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static const char *_ripng_command_name(uint8_t command) {
    switch (command) {
        case RIPNG_COMMAND_REQUEST:  return "Request";
        case RIPNG_COMMAND_RESPONSE: return "Response";
        default:                     return "Unknown";
    }
}

static int _ripng_prefix_is_multicast(const uint8_t *prefix) {
    return 0xff == prefix[0];
}

static int _ripng_prefix_is_link_local(const uint8_t *prefix) {
    return 0xfe == prefix[0] && 0x80 == (prefix[1] & 0xc0);
}

static int _ripng_prefix_is_unspecified(const uint8_t *prefix) {
    size_t i;

    for (i = 0; i < 16; i ++) {
        if (0 != prefix[i]) return 0;
    }
    return 1;
}

static void _print_ripng_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    uint8_t command;
    uint8_t version;
    uint16_t reserved;
    size_t remaining;
    size_t rte_count;
    size_t i;
    const uint8_t *entry;
    const uint8_t *current_next_hop;  /* NULL means the packet originator */
    uint8_t metric;
    uint8_t prefix_len;

    if (!pkt || pkt_len < hdr_len) return;
    if (pkt_len < RIPNG_HDR_LEN) return;

    command = RIPNG_COMMAND(pkt);
    version = RIPNG_VERSION(pkt);
    reserved = RIPNG_RESERVED(pkt);

    printf(
        "command: %u (%s), version: %u, reserved: 0x%04x",
        command, _ripng_command_name(command), version, reserved
    );

    if (0 != reserved) printf(", note: reserved bytes are not zero");
    if (RIPNG_VERSION_1 != version) printf(", note: unsupported version (RFC 2080 defines version 1)");
    if (RIPNG_COMMAND_REQUEST != command && RIPNG_COMMAND_RESPONSE != command)
        printf(", note: unknown command");

    remaining = pkt_len - RIPNG_HDR_LEN;
    rte_count = remaining / RIPNG_RTE_LEN;

    /* whole-table request: exactly one ::/0 RTE with metric 16 */
    if (RIPNG_COMMAND_REQUEST == command && 1 == rte_count) {
        entry = pkt + RIPNG_HDR_LEN;
        if (
            RIPNG_INFINITY == RIPNG_RTE_METRIC(entry) &&
            0 == RIPNG_RTE_PREFIX_LEN(entry) &&
            _ripng_prefix_is_unspecified(RIPNG_RTE_PREFIX(entry))
        ) printf(", note: whole-table request");
    }

    if (0 == rte_count && RIPNG_COMMAND_REQUEST == command) printf(", note: empty request");

    current_next_hop = NULL;

    for (i = 0; i < rte_count; i ++) {
        entry = pkt + RIPNG_HDR_LEN + i * RIPNG_RTE_LEN;
        metric = RIPNG_RTE_METRIC(entry);
        prefix_len = RIPNG_RTE_PREFIX_LEN(entry);

        if (RIPNG_NEXTHOP_METRIC == metric) {
            printf(", rte[%lu]: next-hop: ", (unsigned long)i);
            if (_ripng_prefix_is_unspecified(RIPNG_RTE_PREFIX(entry))) printf(":: (use originator)");
            else {
                print_ipv6(RIPNG_RTE_PREFIX(entry), NULL);
                if (_ripng_prefix_is_link_local(RIPNG_RTE_PREFIX(entry))) current_next_hop = RIPNG_RTE_PREFIX(entry);
                else {
                    printf(" (not link-local, treated as ::)");
                    current_next_hop = NULL;
                }
            }
            if (0 != RIPNG_RTE_ROUTE_TAG(entry) || 0 != prefix_len)
                printf(", note: reserved next-hop fields are not zero");
            continue;
        }

        printf(", rte[%lu]: {prefix: ", (unsigned long)i);
        print_ipv6(RIPNG_RTE_PREFIX(entry), NULL);
        printf("/%u, tag: 0x%04x, metric: %u", prefix_len, RIPNG_RTE_ROUTE_TAG(entry), metric);
        if (RIPNG_INFINITY == metric) printf(" (infinity)");
        printf(", next_hop: ");
        if (NULL != current_next_hop) print_ipv6(current_next_hop, NULL);
        else printf("originator");
        printf("}");

        if (prefix_len > 128) printf(", note: invalid prefix length");
        if (_ripng_prefix_is_multicast(RIPNG_RTE_PREFIX(entry)))
            printf(", note: multicast prefix is not valid in an RTE");
        if (_ripng_prefix_is_link_local(RIPNG_RTE_PREFIX(entry)))
            printf(", note: link-local prefix is not valid in an RTE");
        if (metric > RIPNG_INFINITY) printf(", note: invalid metric");
    }

    printf(", rtes: %lu", (unsigned long)rte_count);
    if (0 != remaining % RIPNG_RTE_LEN)
        printf(", note: %lu trailing byte(s) do not form a complete RTE", (unsigned long)(remaining % RIPNG_RTE_LEN));
}

static void _visualize_ripng_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char command[4];   /* 255'\0' */
    char version[4];
    char reserved[7];  /* 0x0000'\0' */

    if (!pkt || pkt_len < hdr_len) return;
    if (pkt_len < RIPNG_HDR_LEN) return;

    snprintf(command, sizeof(command), "%u", RIPNG_COMMAND(pkt));
    snprintf(version, sizeof(version), "%u", RIPNG_VERSION(pkt));
    snprintf(reserved, sizeof(reserved), "0x%04x", RIPNG_RESERVED(pkt));

    start_printing();
    print_additional_info("Route table entries are not represented in ascii art");
    print_field(RIPNG_COMMAND_LABEL, command, 0);
    print_field(RIPNG_VERSION_LABEL, version, 0);
    print_field(RIPNG_RESERVED_LABEL, reserved, 0);
    end_printing();
}

protocol_info dissect_ripng(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < RIPNG_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_ripng_hdr,
        .visualize_protocol_func = _visualize_ripng_hdr,
        .hdr_len = 0,
        .encap_protocol = NO_ENCAP_PROTO,
        .encap_proto_table_num = NO_ENCAP_PROTO_TABLE
    };
}
