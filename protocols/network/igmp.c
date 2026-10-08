#include "igmp.h"

#include <stdio.h>

#include "../../utils/formats.h"
#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static const char *_igmp_type_name(uint8_t type);
static const char *_igmp_record_type_name(uint8_t record_type);
static void _print_igmp_v3_query_extras(const uint8_t *pkt, size_t pkt_len);
static void _print_igmp_v3_group_records(const uint8_t *pkt, size_t pkt_len);
static void _print_igmp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_igmp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static const char *_igmp_type_name(uint8_t type) {
    switch (type) {
        case IGMP_MEMBERSHIP_QUERY:     return "Membership Query";
        case IGMP_V1_MEMBERSHIP_REPORT: return "IGMPv1 Membership Report";
        case IGMP_V2_MEMBERSHIP_REPORT: return "IGMPv2 Membership Report";
        case IGMP_V3_MEMBERSHIP_REPORT: return "IGMPv3 Membership Report";
        case IGMP_LEAVE_GROUP:          return "Leave Group";
        default:                        return "Unknown";
    }
}

static const char *_igmp_record_type_name(uint8_t record_type) {
    switch (record_type) {
        case IGMP_V3_RECORD_MODE_IS_INCLUDE:   return "MODE_IS_INCLUDE";
        case IGMP_V3_RECORD_MODE_IS_EXCLUDE:   return "MODE_IS_EXCLUDE";
        case IGMP_V3_RECORD_CHANGE_TO_INCLUDE: return "CHANGE_TO_INCLUDE_MODE";
        case IGMP_V3_RECORD_CHANGE_TO_EXCLUDE: return "CHANGE_TO_EXCLUDE_MODE";
        case IGMP_V3_RECORD_ALLOW_NEW_SOURCES: return "ALLOW_NEW_SOURCES";
        case IGMP_V3_RECORD_BLOCK_OLD_SOURCES: return "BLOCK_OLD_SOURCES";
        default:                               return "Unknown";
    }
}

static void _print_igmp_v3_query_extras(const uint8_t *pkt, size_t pkt_len) {
    uint16_t num_sources;
    size_t i;

    num_sources = IGMP_V3_QUERY_NUM_SOURCES(pkt);
    printf(
        ", S: %u, QRV: %u, QQIC: %u, sources: %u",
        IGMP_V3_QUERY_S(pkt),
        IGMP_V3_QUERY_QRV(pkt),
        IGMP_V3_QUERY_QQIC(pkt),
        num_sources
    );

    for (i = 0; i < (size_t)num_sources; i ++) {
        if (IGMP_V3_QUERY_HDR_LEN + i * 4 + 4 > pkt_len) break;
        printf(", source %lu: ", (unsigned long)(i + 1));
        print_ipv4(IGMP_V3_QUERY_SOURCE(pkt, i));
    }
}

static void _print_igmp_v3_group_records(const uint8_t *pkt, size_t pkt_len) {
    uint16_t num_records;
    size_t offset;
    size_t i, j;

    num_records = IGMP_V3_REPORT_NUM_RECORDS(pkt);
    printf(", group_records: %u", num_records);

    offset = IGMP_V3_REPORT_HDR_LEN;
    for (i = 0; i < (size_t)num_records; i ++) {
        uint16_t num_sources;
        uint8_t aux_len;
        size_t record_len;

        if (offset + IGMP_V3_RECORD_HDR_LEN > pkt_len) return;

        num_sources = IGMP_V3_RECORD_NUM_SOURCES(pkt + offset);
        aux_len = IGMP_V3_RECORD_AUX_LEN(pkt + offset);
        record_len = IGMP_V3_RECORD_HDR_LEN + (size_t)num_sources * 4 + (size_t)aux_len * 4;

        printf(
            ", record %lu: {type: 0x%02x (%s), sources: %u, mcast: ",
            (unsigned long)(i + 1),
            IGMP_V3_RECORD_TYPE(pkt + offset),
            _igmp_record_type_name(IGMP_V3_RECORD_TYPE(pkt + offset)),
            num_sources
        );
        print_ipv4(IGMP_V3_RECORD_MCAST_ADDR(pkt + offset));

        for (j = 0; j < (size_t)num_sources; j ++) {
            size_t src_offset = offset + IGMP_V3_RECORD_HDR_LEN + j * 4;

            if (src_offset + 4 > pkt_len) break;
            printf(", source %lu: ", (unsigned long)(j + 1));
            print_ipv4(IGMP_V3_RECORD_SOURCE(pkt + offset, j));
        }
        printf("}");

        if (offset + record_len > pkt_len) return;
        offset += record_len;
    }
}

static void _print_igmp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    uint8_t type;

    if (!pkt || pkt_len < hdr_len) return;
    if (pkt_len < IGMP_HDR_LEN) return;

    type = IGMP_TYPE(pkt);
    printf(
        "type: 0x%02x (%s), max_resp: %u, checksum: 0x%04x, group: ",
        type,
        _igmp_type_name(type),
        IGMP_MAX_RESP(pkt),
        IGMP_CHECKSUM(pkt)
    );
    print_ipv4(IGMP_GROUP_ADDR(pkt));

    if (IGMP_MEMBERSHIP_QUERY == type) {
        if (pkt_len >= IGMP_V3_QUERY_HDR_LEN) _print_igmp_v3_query_extras(pkt, pkt_len);
    }
    else if (IGMP_V3_MEMBERSHIP_REPORT == type) {
        if (pkt_len >= IGMP_V3_REPORT_HDR_LEN) _print_igmp_v3_group_records(pkt, pkt_len);
    }
}

static void _visualize_igmp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char type[5];  /* 0x00'\0' */
    char max_resp[4];  /* 255'\0' */
    char checksum[7];  /* 0x0000'\0' */
    char group_addr[IP_ADDR_STR_LEN];

    if (!pkt || pkt_len < hdr_len) return;
    if (pkt_len < IGMP_HDR_LEN) return;

    snprintf(type, sizeof(type), "0x%02x", IGMP_TYPE(pkt));
    snprintf(max_resp, sizeof(max_resp), "%u", IGMP_MAX_RESP(pkt));
    snprintf(checksum, sizeof(checksum), "0x%04x", IGMP_CHECKSUM(pkt));
    snprintf(group_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(IGMP_GROUP_ADDR(pkt)));

    start_printing();
    print_additional_info("IGMPv3 extension fields and group records not represented in ascii art");
    print_field(IGMP_TYPE_LABEL, type, 0);
    print_field(IGMP_MAX_RESP_LABEL, max_resp, 0);
    print_field(IGMP_CHECKSUM_LABEL, checksum, 0);
    print_field(IGMP_GROUP_ADDR_LABEL, group_addr, 0);
    end_printing();
}

protocol_info dissect_igmp(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < IGMP_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_igmp_hdr,
        .visualize_protocol_func = _visualize_igmp_hdr,
        .hdr_len = 0,
        .encap_protocol = NO_ENCAP_PROTO,
        .encap_proto_table_num = NO_ENCAP_PROTO_TABLE
    };
}
