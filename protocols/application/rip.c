#include "rip.h"

#include <stdio.h>

#include "../../utils/formats.h"
#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static const char *_rip_command_name(uint8_t command);
static const char *_rip_infer_alg(uint8_t auth_data_len);
static unsigned _rip_prefix_len(uint32_t mask);
static void _rip_print_hex(const uint8_t *pkt, size_t pkt_len, size_t offset, size_t len);
static void _rip_print_route_entry(const uint8_t *entry);
static void _rip_print_entries(const uint8_t *pkt, size_t pkt_len, size_t start, size_t end);
static void _rip_print_simple_auth(const uint8_t *pkt, size_t pkt_len);
static void _rip_print_crypto_auth(const uint8_t *pkt, size_t pkt_len);
static void _print_rip_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_rip_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static const char *_rip_command_name(uint8_t command) {
    switch (command) {
        case RIP_COMMAND_REQUEST:  return "Request";
        case RIP_COMMAND_RESPONSE: return "Response";
        default:                   return "Unknown";
    }
}

/* The algorithm is never on the wire; infer a candidate from the digest size (RFC 4822). */
static const char *_rip_infer_alg(uint8_t auth_data_len) {
    switch (auth_data_len) {
        case 16: return "Keyed-MD5";
        case 20: return "HMAC-SHA-1";
        case 32: return "HMAC-SHA-256";
        case 48: return "HMAC-SHA-384";
        case 64: return "HMAC-SHA-512";
        default: return "unknown";
    }
}

static unsigned _rip_prefix_len(uint32_t mask) {
    unsigned len = 0;

    while (0 != (mask & 0x80000000u)) {
        len ++;
        mask <<= 1;
    }
    return len;
}

static void _rip_print_hex(const uint8_t *pkt, size_t pkt_len, size_t offset, size_t len) {
    size_t i;

    for (i = 0; i < len && offset + i < pkt_len; i ++) {
        printf("%02x", pkt[offset + i]);
    }
    if (offset + len > pkt_len) printf("(truncated)");
}

static void _rip_print_route_entry(const uint8_t *entry) {
    uint32_t next_hop = RIP_ENTRY_NEXTHOP(entry);

    printf("{afi: %u, tag: 0x%04x, addr: ", RIP_ENTRY_AFI(entry), RIP_ENTRY_TAG(entry));
    print_ipv4(RIP_ENTRY_ADDR(entry));
    printf("/%u, metric: %u, next_hop: ", _rip_prefix_len(RIP_ENTRY_MASK(entry)), RIP_ENTRY_METRIC(entry));
    if (0 == next_hop) printf("self");
    else print_ipv4(next_hop);
    printf("}");
}

static void _rip_print_entries(const uint8_t *pkt, size_t pkt_len, size_t start, size_t end) {
    size_t offset = start;
    size_t count = 0;

    if (end > pkt_len) end = pkt_len;

    while (offset + RIP_ENTRY_LEN <= end) {
        printf(", route %lu: ", (unsigned long)(count + 1));
        _rip_print_route_entry(pkt + offset);
        offset += RIP_ENTRY_LEN;
        count ++;
    }
    printf(", routes: %lu", (unsigned long)count);
    if (offset < end) printf(", note: %lu trailing byte(s) (< %d)", (unsigned long)(end - offset), RIP_ENTRY_LEN);
}

static void _rip_print_simple_auth(const uint8_t *pkt, size_t pkt_len) {
    size_t i;

    printf(", auth: simple_text, password: ");
    for (i = 8; i < RIP_HEADER_WITH_AUTH_LEN && i < pkt_len; i ++) {
        putchar((pkt[i] >= 0x20 && pkt[i] < 0x7f) ? (char)pkt[i] : '.');
    }
    _rip_print_entries(pkt, pkt_len, RIP_HEADER_WITH_AUTH_LEN, pkt_len);
}

static void _rip_print_crypto_auth(const uint8_t *pkt, size_t pkt_len) {
    uint16_t packet_len = RIP_AUTH_PACKET_LEN(pkt);
    uint8_t key_id = RIP_AUTH_KEY_ID(pkt);
    uint8_t auth_data_len = RIP_AUTH_DATA_LEN(pkt);
    size_t trailer_offset = (size_t)packet_len;

    printf(
        ", auth: cryptographic, packet_len: %u, key_id: %u, auth_data_len: %u (inferred: %s), sequence: %u",
        packet_len, key_id, auth_data_len, _rip_infer_alg(auth_data_len), RIP_AUTH_SEQUENCE(pkt)
    );

    if (0 != RIP_AUTH_RESERVED_1(pkt) || 0 != RIP_AUTH_RESERVED_2(pkt))
        printf(", note: reserved bytes are not zero");

    if (packet_len < RIP_HEADER_WITH_AUTH_LEN) {
        printf(", error: packet_len (%u) is below the %d-byte header", packet_len, RIP_HEADER_WITH_AUTH_LEN);
        return;
    }
    if (0 != (packet_len - RIP_HEADER_WITH_AUTH_LEN) % RIP_ENTRY_LEN)
        printf(", note: (packet_len - header) is not a multiple of %d", RIP_ENTRY_LEN);

    _rip_print_entries(pkt, pkt_len, RIP_HEADER_WITH_AUTH_LEN, trailer_offset);

    if (trailer_offset + RIP_AUTH_TRAILER_MARKER_LEN <= pkt_len) {
        uint16_t marker_afi = RIP_AUTH_TRAILER_AFI(pkt, trailer_offset);
        uint16_t marker_tag = RIP_AUTH_TRAILER_TAG_FIELD(pkt, trailer_offset);
        size_t data_offset = trailer_offset + RIP_AUTH_TRAILER_MARKER_LEN;

        if (RIP_AFI_AUTHENTICATION != marker_afi || RIP_AUTH_TRAILER_TAG != marker_tag)
            printf(", note: unexpected auth trailer marker (0x%04x, 0x%04x)", marker_afi, marker_tag);

        printf(", auth_data: ");
        _rip_print_hex(pkt, pkt_len, data_offset, auth_data_len);

        if (data_offset + (size_t)auth_data_len > pkt_len)
            printf(", error: auth_data truncated");
    }
    else printf(", error: auth trailer is missing or truncated");
}

static void _print_rip_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    uint8_t version;

    if (!pkt || pkt_len < hdr_len) return;
    if (pkt_len < RIP_HDR_LEN) return;

    printf(
        "command: %u (%s), version: %u, routing_domain: %u",
        RIP_COMMAND(pkt), _rip_command_name(RIP_COMMAND(pkt)), RIP_VERSION(pkt), RIP_ROUTING_DOMAIN(pkt)
    );

    version = RIP_VERSION(pkt);
    if (RIP_VERSION_2 == version && pkt_len >= RIP_HEADER_WITH_AUTH_LEN) {
        if (RIP_AFI_AUTHENTICATION == RIP_AUTH_AFI(pkt)) {
            uint16_t auth_type = RIP_AUTH_TYPE(pkt);

            if (RIP_AUTH_TYPE_CRYPTO == auth_type) {
                _rip_print_crypto_auth(pkt, pkt_len);
                return;
            }
            if (RIP_AUTH_TYPE_SIMPLE == auth_type) {
                _rip_print_simple_auth(pkt, pkt_len);
                return;
            }
            printf(", auth_type: %u (unsupported)", auth_type);
        }
    }

    _rip_print_entries(pkt, pkt_len, RIP_HDR_LEN, pkt_len);
}

static void _visualize_rip_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char command[16];
    char version[4];
    char routing_domain[6];
    char auth_type[6];
    char packet_len[6];
    char key_id[4];
    char auth_data_len[4];
    char sequence[11];
    uint8_t version_val;
    int has_crypto_auth = 0;

    if (!pkt || pkt_len < hdr_len) return;
    if (pkt_len < RIP_HDR_LEN) return;

    version_val = RIP_VERSION(pkt);
    if (
        RIP_VERSION_2 == version_val &&
        pkt_len >= RIP_HEADER_WITH_AUTH_LEN &&
        RIP_AFI_AUTHENTICATION == RIP_AUTH_AFI(pkt) &&
        RIP_AUTH_TYPE_CRYPTO == RIP_AUTH_TYPE(pkt)
    ) has_crypto_auth = 1;

    snprintf(command, sizeof(command), "%u", RIP_COMMAND(pkt));
    snprintf(version, sizeof(version), "%u", version_val);
    snprintf(routing_domain, sizeof(routing_domain), "%u", RIP_ROUTING_DOMAIN(pkt));

    start_printing();
    print_additional_info("Route entries and authentication trailer are not represented in ascii art");
    print_field(RIP_COMMAND_LABEL, command, 0);
    print_field(RIP_VERSION_LABEL, version, 0);
    print_field(RIP_ROUTING_DOMAIN_LABEL, routing_domain, 0);

    if (has_crypto_auth) {
        snprintf(auth_type, sizeof(auth_type), "%u", RIP_AUTH_TYPE(pkt));
        snprintf(packet_len, sizeof(packet_len), "%u", RIP_AUTH_PACKET_LEN(pkt));
        snprintf(key_id, sizeof(key_id), "%u", RIP_AUTH_KEY_ID(pkt));
        snprintf(auth_data_len, sizeof(auth_data_len), "%u", RIP_AUTH_DATA_LEN(pkt));
        snprintf(sequence, sizeof(sequence), "%u", RIP_AUTH_SEQUENCE(pkt));

        print_field(RIP_AUTH_TYPE_LABEL, auth_type, 0);
        print_field(RIP_PACKET_LEN_LABEL, packet_len, 0);
        print_field(RIP_KEY_ID_LABEL, key_id, 0);
        print_field(RIP_AUTH_DATA_LEN_LABEL, auth_data_len, 0);
        print_field(RIP_SEQUENCE_LABEL, sequence, 0);
    }
    end_printing();
}

protocol_info dissect_rip(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < RIP_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_rip_hdr,
        .visualize_protocol_func = _visualize_rip_hdr,
        .hdr_len = 0,
        .encap_protocol = NO_ENCAP_PROTO,
        .encap_proto_table_num = NO_ENCAP_PROTO_TABLE
    };
}
