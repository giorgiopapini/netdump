#include <stdio.h>
#include <arpa/inet.h>

#include "icmp.h"
#include "../../utils/visualizer.h"


size_t icmp_hdr_len(const uint8_t *pkt) { return ICMP_LEN; }

void print_icmp_hdr(const uint8_t *pkt) {
    printf("ICMP echo ");
    uint8_t type = TYPE(pkt);

    if (ICMP_REQUEST == type) printf("request");
    else if (ICMP_REPLY == type) printf("reply");

    printf(", id %d, seq %d", ID(pkt), SEQUENCE(pkt));
}

void visualize_icmp_hdr(const uint8_t *pkt) {
    char type[3];
    char code[3];
    char checksum[7];  /* 0x0000'\0' are 7 chars */
    char id[7];  /* 0x0000'\0' are 7 chars */
    char seq[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */

    snprintf(type, sizeof(type), "%d", TYPE(pkt));
    snprintf(code, sizeof(code), "%d", CODE(pkt));
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(CHECKSUM(pkt)));
    snprintf(id, sizeof(id), "0x%04x", ntohs(ID(pkt)));
    snprintf(seq, sizeof(seq), "%d", ntohs(SEQUENCE(pkt)));

    start_printing();
    print_hdr_info(ICMP_HEADER_LABEL, NULL);
    print_field(TYPE_LABEL, type, 0);
    print_field(CODE_LABEL, code, 0);
    print_field(CHECKSUM_LABEL, checksum, 0);
    print_field(ID_LABEL, id, 0);
    print_field(SEQUENCE_LABEL, seq, 0);
    end_printing();
}