#include <stdio.h>
#include <string.h>

#include "dns.h"
#include "../../utils/visualizer.h"


int extract_domain_name(const uint8_t *payload, int offset, char *domain) {
    int i = 0, len, start_offset = offset;
    uint16_t pointer_offset;

    while ((len = payload[offset]) > 0) {
        if (len & 0xC0) {
            pointer_offset = ((payload[offset] & 0x3F) << 8) | payload[offset + 1];
            offset += 2;

            i += extract_domain_name(payload, pointer_offset, domain + i);
            return offset - start_offset;
        } 
        else {
            if (i > 0) domain[i++] = '.';
            offset ++;
            memcpy(&domain[i], &payload[offset], len);
            i += len;
            offset += len;
        }
    }
    domain[i] = '\0';
    return (offset - start_offset) + 1;
}

void extract_srv_record(const uint8_t *payload, int offset, int data_len) {
    char target[256];
    int target_offset;

    if (data_len < 6) return;
    
    target_offset = offset + 6;
    extract_domain_name(payload, target_offset, target);

    printf(", priority: %u", ntohs(*(unsigned short *)(payload + offset)));
    printf(", weight: %u", ntohs(*(unsigned short *)(payload + offset + 2)));
    printf(", port: %u", ntohs(*(unsigned short *)(payload + offset + 4)));
    printf(", target: %s", target);
}

void print_dns_hdr(const uint8_t *pkt, uint32_t len) {
    char flags[128] = "";  /* IMPORTANT! Initialize flags to empty str, otherwiese strcat could lead to undefined behaviours */
    char opcode_str[16];
    char rcode_str[16];
    char domain[256];
    int offset;
    int i;

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

    /* print queries */
    offset = DNS_HDR_LEN;
    for (i = 0; i < ntohs(DNS_QUESTIONS(pkt)); i ++) {
        offset += extract_domain_name(pkt, offset, domain);
        printf(
            ", query %d: {name: %s, type: %u, class: %u}",
            i + 1,
            domain,
            ntohs(*(uint16_t *)(pkt + offset)),
            ntohs(*(uint16_t *)(pkt + offset + 2))
        );
        offset += 4;
    }

    /* print answers */
    for (i = 0; i < ntohs(DNS_ANSWER_RRS(pkt)); i++) {
        uint16_t type, class, data_len;
        uint32_t ttl;
        
        offset += extract_domain_name(pkt, offset, domain);

        type = ntohs(*(uint16_t *)(pkt + offset));
        class = ntohs(*(uint16_t *)(pkt + offset + 2));
        ttl = ntohl(*(uint32_t *)(pkt + offset + 4));
        data_len = ntohs(*(uint16_t *)(pkt + offset + 8));

        printf(
            ", answer %d: {name: %s, type: %u, class: %u, ttl: %u, data_length: %u",
            i + 1,
            domain,
            type,
            class,
            ttl,
            data_len
        );
        
        offset += 10;
        if (33 == type) extract_srv_record(pkt, offset, data_len);
        printf("}");

        offset += data_len;
    }

    /* print additional records */
    for (i = 0; i < ntohs(DNS_ADDITIONAL_RRS(pkt)); i ++) {
        uint8_t rdlength = ntohs(*(uint8_t *)(pkt + offset + 8));

        printf(
            ", additional_record %d: {name: <Root>, type: %u, udp_payload_size: %u, higher_bits_in_rcode: %u, EDNS0_v: %u, Z: 0x%04x, rdlength: %u",
            i + 1,
            ntohs(*(uint16_t *)(pkt + offset + 1)),
            ntohs(*(uint16_t *)(pkt + offset + 3)),
            *(pkt + offset + 5),
            *(pkt + offset + 6),
            ntohs(*(uint16_t *)(pkt + offset + 7)),
            rdlength
        );
        offset += 10;

        if (rdlength > 0) {
            printf(", rdata: (");
            for (int j = 0; j < rdlength; j++) printf("%02x ", pkt[offset + j]);
            printf(")");
        }
        printf("}");

        offset += rdlength;
    }
}

void visualize_dns_hdr(const uint8_t *pkt, uint32_t len) {
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
    print_additional_info("Queries, Answers and Additional Records not represented in ascii art");
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

protocol_info dissect_dns(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_dns_hdr, visualize_dns_hdr);
    return NO_ENCAP_PROTO;
}