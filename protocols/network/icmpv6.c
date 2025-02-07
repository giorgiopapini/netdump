#include <stdio.h>

#include "icmpv6.h"
#include "../../utils/visualizer.h"


void print_type(uint8_t type) {
    switch (type) {
        case ICMPV6_DEST_UNREACH_CODE:      printf("destination unreachable"); break;
        case ICMPV6_TOO_LARGE_CODE:         printf("size greater than MTU"); break;
        case ICMPV6_TIME_EXCEED_CODE:       printf("time exceeded"); break;
        case ICMPV6_PARAM_ERR_CODE:         printf("parameter problem"); break;
        case ICMPV6_ECHO_REQUEST_CODE:      printf("echo request"); break;
        case ICMPV6_ECHO_REPLY_CODE:        printf("echo reply"); break;
        case ICMPV6_MULTI_QUERY_CODE:       printf("multicast listener query"); break;
        case ICMPV6_MULTI_REPORT_CODE:      printf("multicast listener report"); break;
        case ICMPV6_MULTI_DONE_CODE:        printf("multicast listener done"); break;
        case ICMPV6_ROUTER_REQUEST_CODE:    printf("router solicitation"); break;
        case ICMPV6_ROUTER_ANNOUNCE_CODE:   printf("router advertisement"); break;
        case ICMPV6_ADJ_ROUTER_REQ_CODE:    printf("neighbor solicitation"); break;
        case ICMPV6_ADJ_ROUTER_ANN_CODE:    printf("neighbor advertisement"); break;
        case ICMPV6_MESSAGE_REDIR_CODE:     printf("redirect message"); break;
        default: break;
    }
    printf(" (%u)", type);
}

void print_icmpv6_hdr(const uint8_t *pkt) {
    print_type(ICMPV6_TYPE(pkt));

    printf(
        ", code: %u, checksum: 0x%04x",
        ICMPV6_CODE(pkt),
        ntohs(ICMPV6_CHECKSUM(pkt))
    );

    if (ICMPV6_ECHO_REQUEST_CODE == ICMPV6_TYPE(pkt) || ICMPV6_ECHO_REPLY_CODE == ICMPV6_TYPE(pkt)) {
        printf(
            ", id: 0x%04x, seq: %u", 
            ntohs(ICMPV6_ID(pkt)), 
            ntohs(ICMPV6_SEQ(pkt))
        );
    }
}

void visualize_icmpv6_hdr(const uint8_t *pkt) {
    char type[4];  /* 4 bits --> max: 255'\0' 4 chars */
    char code[4];
    char checksum[7];  /* 0x0000'\0' 7 chars */

    snprintf(type, sizeof(type), "%u", ICMPV6_TYPE(pkt));
    snprintf(code, sizeof(code), "%u", ICMPV6_CODE(pkt));
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(ICMPV6_CHECKSUM(pkt)));

    start_printing();
    print_hdr_info(ICMPV6_HEADER_LABEL, NULL);
    print_field(ICMPV6_TYPE_LABEL, type, 0);
    print_field(ICMPV6_CODE_LABEL, code, 0);
    print_field(ICMPV6_CHECKSUM_LABEL, checksum, 0);

    if (ICMPV6_ECHO_REQUEST_CODE == ICMPV6_TYPE(pkt) || ICMPV6_ECHO_REPLY_CODE == ICMPV6_TYPE(pkt)) {
        char id[7];  /* 0x0000'\0' 7 chars */
        char seq[6];  /* 65535'\0' 6 chars */

        snprintf(id, sizeof(id), "0x%04x", ntohs(ICMPV6_ID(pkt)));
        snprintf(seq, sizeof(seq), "%u", ntohs(ICMPV6_SEQ(pkt)));

        print_field(ICMPV6_ID_LABEL, id, 0);
        print_field(ICMPV6_SEQ_LABEL, seq, 0);
    }
    end_printing();
}

protocol_info dissect_icmpv6(const uint8_t *pkt, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_icmpv6_hdr, visualize_icmpv6_hdr);
    return NO_ENCAP_PROTO;
}