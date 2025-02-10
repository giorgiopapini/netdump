#include <stdio.h>
#include <string.h>

#include "dns.h"
#include "../../utils/visualizer.h"


void print_dns_hdr(const uint8_t *pkt) {
    char flags[128] = "";  /* IMPORTANT! Initialize flags to empty str, otherwiese strcat could lead to undefined behaviours */
    char opcode_str[16];
    char rcode_str[16];

    printf("transaction_id: 0x%04x", ntohs(DNS_TRANSACTION_ID(pkt)));

    printf(", flags: [");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_QR) strcat(flags, "RES, ");
    else strcat(flags, "QUERY, ");
    
    snprintf(opcode_str, sizeof(opcode_str), "OPCODE: %u, ", (ntohs(DNS_FLAGS(pkt)) & DNS_OPCODE) >> 11);
    strcat(flags, opcode_str);

    if (ntohs(DNS_FLAGS(pkt)) & DNS_AA) strcat(flags, "AA, ");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_TC) strcat(flags, "TC, ");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_RD) strcat(flags, "RD, ");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_RA) strcat(flags, "RA, ");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_AD) strcat(flags, "AD, ");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_CD) strcat(flags, "CD, ");
    if (ntohs(DNS_FLAGS(pkt)) & DNS_QR) {
        snprintf(rcode_str, sizeof(rcode_str), "RCODE: %u, ", ntohs(DNS_FLAGS(pkt)) & DNS_RCODE);
        strcat(flags, rcode_str);  /* RCODE is there only if it's a reply, not a query */
    }

    flags[strlen(flags) - 2] = '\0';    /* remove last ", " chars */
    printf("%s]", flags);

    printf(
        ", questions: %u, answer_rrs: %u, auth_rrs: %u, additional_rrs: %u",
        ntohs(DNS_QUESTIONS(pkt)),
        ntohs(DNS_ANSWER_RRS(pkt)),
        ntohs(DNS_AUTH_RRS(pkt)),
        ntohs(DNS_ADDITIONAL_RRS(pkt))
    );
}

void visualize_dns_hdr(const uint8_t *pkt) {
    char transaction_id[7];  /* 0x0000'\0' 7 chars */
    char qr[2];
    char opcode[3];
    char aa[2];
    char tc[2];
    char rd[2];
    char ra[2];
    char ad[2];
    char cd[2];
    char rcode[3];
    char questions[6];  /* 65535'\0' 6 chars */
    char answer_rrs[6];
    char auth_rrs[6];
    char additional_rrs[6];

    snprintf(transaction_id, sizeof(transaction_id), "0x%04x", ntohs(DNS_TRANSACTION_ID(pkt)));
    snprintf(qr, sizeof(qr), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_QR) ? 1 : 0);
    snprintf(opcode, sizeof(opcode), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_OPCODE) >> 11);
    snprintf(aa, sizeof(aa), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_AA) ? 1 : 0);
    snprintf(tc, sizeof(tc), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_TC) ? 1 : 0);
    snprintf(rd, sizeof(rd), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_RD) ? 1 : 0);
    snprintf(ra, sizeof(ra), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_RA) ? 1 : 0);
    snprintf(ad, sizeof(ad), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_AD) ? 1 : 0);
    snprintf(cd, sizeof(cd), "%u", (ntohs(DNS_FLAGS(pkt)) & DNS_CD) ? 1 : 0);
    if (ntohs(DNS_FLAGS(pkt)) & DNS_QR) snprintf(rcode, sizeof(rcode), "%u", ntohs(DNS_FLAGS(pkt)) & DNS_RCODE);
    snprintf(questions, sizeof(questions), "%u", ntohs(DNS_QUESTIONS(pkt)));
    snprintf(answer_rrs, sizeof(answer_rrs), "%u", ntohs(DNS_ANSWER_RRS(pkt)));
    snprintf(auth_rrs, sizeof(auth_rrs), "%u", ntohs(DNS_AUTH_RRS(pkt)));
    snprintf(additional_rrs, sizeof(additional_rrs), "%u", ntohs(DNS_ADDITIONAL_RRS(pkt)));

    start_printing();
    print_hdr_info(DNS_HEADER_LABEL, NULL);
    print_field(DNS_TRANSACTION_ID_LABEL, transaction_id, 0);
    print_field(DNS_QR_LABEL, qr, 0);
    print_field(DNS_OPCODE_LABEL, opcode, 0);
    print_field(DNS_AA_LABEL, aa, 0);
    print_field(DNS_TC_LABEL, tc, 0);
    print_field(DNS_RD_LABEL, rd, 0);
    print_field(DNS_RA_LABEL, ra, 0);
    print_field(DNS_AD_LABEL, ad, 0);
    print_field(DNS_CD_LABEL, cd, 0);
    if (ntohs(DNS_FLAGS(pkt)) & DNS_QR) print_field(DNS_RCODE_LABEL, rcode, 0);
    print_field(DNS_QUESTIONS_LABEL, questions, 0);
    print_field(DNS_ANSWER_RRS_LABEL, answer_rrs, 0);
    print_field(DNS_AUTH_RRS_LABEL, auth_rrs, 0);
    print_field(DNS_ADDITIONAL_RRS_LABEL, additional_rrs, 0);
    end_printing();
}

protocol_info dissect_dns(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_dns_hdr, visualize_dns_hdr);
    return NO_ENCAP_PROTO;
}