#include "icmp.h"

#include <stdio.h>

#include "../../utils/visualizer.h"
#include "../../utils/protocol.h"


static void _print_icmp_hdr(const uint8_t *pkt, size_t pkt_len);
static void _visualize_icmp_hdr(const uint8_t *pkt, size_t pkt_len);

static void _print_icmp_hdr(const uint8_t *pkt, size_t pkt_len) {
    uint8_t type;

    if (!pkt || pkt_len < ICMP_HDR_LEN) return;

    printf("ICMP echo ");
    type = ICMP_TYPE(pkt);

    if (ICMP_REQUEST == type) printf("request");
    else if (ICMP_REPLY == type) printf("reply");

    printf(", id %u, seq %u", ICMP_ID(pkt), ICMP_SEQUENCE(pkt));
}

static void _visualize_icmp_hdr(const uint8_t *pkt, size_t pkt_len) {
    char type[4];
    char code[4];
    char checksum[7];  /* 0x0000'\0' are 7 chars */
    char id[7];  /* 0x0000'\0' are 7 chars */
    char seq[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    
    if (!pkt || pkt_len < ICMP_HDR_LEN) return;
    
    snprintf(type, sizeof(type), "%u", ICMP_TYPE(pkt));
    snprintf(code, sizeof(code), "%u", ICMP_CODE(pkt));
    snprintf(checksum, sizeof(checksum), "0x%04x", ICMP_CHECKSUM(pkt));
    snprintf(id, sizeof(id), "0x%04x", ICMP_ID(pkt));
    snprintf(seq, sizeof(seq), "%u", ICMP_SEQUENCE(pkt));

    start_printing();
    print_field(ICMP_TYPE_LABEL, type, 0);
    print_field(ICMP_CODE_LABEL, code, 0);
    print_field(ICMP_CHECKSUM_LABEL, checksum, 0);
    print_field(ICMP_ID_LABEL, id, 0);
    print_field(ICMP_SEQUENCE_LABEL, seq, 0);
    end_printing();
}

protocol_info dissect_icmp(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    if (!pkt || pkt_len < ICMP_HDR_LEN) return NO_ENCAP_PROTO;

    SHOW_OUTPUT(pkt, pkt_len, fmt, _print_icmp_hdr, _visualize_icmp_hdr);
    return NO_ENCAP_PROTO;
}