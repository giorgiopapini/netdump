#include "dns.h"

#include <stdio.h>
#include <string.h>

#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static size_t _extract_domain_name(const uint8_t *pkt, size_t offset, size_t pkt_len, char *domain);
static void _extract_srv_record(const uint8_t *payload, size_t offset, size_t data_len);
static void _print_dns_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_dns_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static size_t _extract_domain_name(const uint8_t *pkt, size_t offset, size_t pkt_len, char *domain) {
    size_t start_offset = offset;
    size_t label_len;
    size_t pointer;
    size_t domain_pos = 0;

    if (offset >= pkt_len) return 0;

    while (offset < pkt_len) {
        label_len = pkt[offset ++];
        if (0 == label_len) break;

        if (label_len & 0xC0) {
            if (offset >= pkt_len) return 0;
            pointer = ((label_len & 0x3F) << 8) | pkt[offset ++];
            if (pointer >= pkt_len) return 0;
            _extract_domain_name(pkt, pointer, pkt_len, domain + domain_pos);
            return offset - start_offset + 1;
        }

        if (offset > pkt_len - label_len) return 0;

        if (0 != domain_pos) domain[domain_pos ++] = '.';
        memcpy(domain + domain_pos, pkt + offset, label_len);
        domain_pos += label_len;
        offset += label_len;
    }

    domain[domain_pos] = '\0';
    return offset - start_offset;
}

static void _extract_srv_record(const uint8_t *payload, size_t offset, size_t data_len) {
    char target[256];
    size_t target_offset;

    if (data_len < 6) return;
    
    target_offset = offset + 6;
    
    _extract_domain_name(payload, target_offset, data_len, target);

    printf(", priority: %u", payload[offset]);
    printf(", weight: %u", payload[offset + 2]);
    printf(", port: %u", payload[offset + 4]);
    printf(", target: %s", target);
}

static void _print_dns_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char opcode_str[16];
    char rcode_str[16];
    char domain[256];
    uint8_t rdlength;
    uint16_t type, class, data_len;
    uint32_t ttl;
    size_t offset;
    size_t i, j;
    char flags[128] = "";  /* IMPORTANT! Initialize flags to empty str, otherwiese strcat could lead to undefined behaviours */
    
    if (!pkt || pkt_len < hdr_len) return;

    printf("transaction_id: 0x%04x", DNS_TRANSACTION_ID(pkt));

    printf(", flags: [");
    if (DNS_FLAGS(pkt) & DNS_QR) strcat(flags, "RES, ");
    else strcat(flags, "QUERY, ");
    
    snprintf(opcode_str, sizeof(opcode_str), "OPCODE: %u, ", (DNS_FLAGS(pkt) & DNS_OPCODE) >> 11);
    strcat(flags, opcode_str);

    if (DNS_FLAGS(pkt) & DNS_AA) strcat(flags, "AA, ");
    if (DNS_FLAGS(pkt) & DNS_TC) strcat(flags, "TC, ");
    if (DNS_FLAGS(pkt) & DNS_RD) strcat(flags, "RD, ");
    if (DNS_FLAGS(pkt) & DNS_RA) strcat(flags, "RA, ");
    if (DNS_FLAGS(pkt) & DNS_AD) strcat(flags, "AD, ");
    if (DNS_FLAGS(pkt) & DNS_CD) strcat(flags, "CD, ");
    if (DNS_FLAGS(pkt) & DNS_QR) {
        snprintf(rcode_str, sizeof(rcode_str), "RCODE: %u, ", DNS_FLAGS(pkt) & DNS_RCODE);
        strcat(flags, rcode_str);  /* RCODE is there only if it's a reply, not a query */
    }

    flags[strlen(flags) - 2] = '\0';    /* remove last ", " chars */
    printf("%s]", flags);

    printf(
        ", questions: %u, answer_rrs: %u, auth_rrs: %u, additional_rrs: %u",
        DNS_QUESTIONS(pkt),
        DNS_ANSWER_RRS(pkt),
        DNS_AUTH_RRS(pkt),
        DNS_ADDITIONAL_RRS(pkt)
    );

    /* print queries */
    offset = hdr_len;
    for (i = 0; i < (size_t)DNS_QUESTIONS(pkt); i ++) {
        offset += _extract_domain_name(pkt, offset, pkt_len, domain);
        if (offset > pkt_len - 4) return;
        printf(
            ", query %ld: {name: %s, type: %u, class: %u}",
            i + 1,
            domain,
            ((uint16_t)((uint16_t)(pkt)[offset] << 8) | (uint16_t)(pkt)[offset + 1]),
            ((uint16_t)((uint16_t)(pkt)[offset + 2] << 8) | (uint16_t)(pkt)[offset + 3])
        );
        offset += 4;
    }

    /* print answers */
    for (i = 0; i < (size_t)DNS_ANSWER_RRS(pkt); i ++) {
        offset += _extract_domain_name(pkt, offset, pkt_len, domain);

        if (offset > pkt_len - 10) return;

        type = ((uint16_t)((uint16_t)(pkt)[offset] << 8) | (uint16_t)(pkt)[offset + 1]),
        class = ((uint16_t)((uint16_t)(pkt)[offset + 2] << 8) | (uint16_t)(pkt)[offset + 3]),
        ttl = ((uint16_t)((uint16_t)(pkt)[offset + 4] << 8) | (uint16_t)(pkt)[offset + 5]),
        data_len = ((uint16_t)((uint16_t)(pkt)[offset + 6] << 8) | (uint16_t)(pkt)[offset + 7]),

        printf(
            ", answer %ld: {name: %s, type: %u, class: %u, ttl: %u, data_length: %u",
            i + 1,
            domain,
            type,
            class,
            ttl,
            data_len
        );
        
        offset += 10;
        if (33 == type) _extract_srv_record(pkt, offset, data_len);
        printf("}");

        if (offset > pkt_len - data_len) return;
        offset += data_len;
    }

    /* print additional records */
    for (i = 0; i < (size_t)DNS_ADDITIONAL_RRS(pkt); i ++) {
        if (offset > pkt_len - 9) return;
        rdlength = pkt[offset + 8];

        printf(
            ", additional_record %ld: {name: <Root>, type: %u, udp_payload_size: %u, higher_bits_in_rcode: %u, EDNS0_v: %u, Z: 0x%04x, rdlength: %u",
            i + 1,
            ((uint16_t)((uint16_t)(pkt)[offset + 1] << 8) | (uint16_t)(pkt)[offset + 2]),
            ((uint16_t)((uint16_t)(pkt)[offset + 3] << 8) | (uint16_t)(pkt)[offset + 4]),
            pkt[offset + 5],
            pkt[offset + 6],
            ((uint16_t)((uint16_t)(pkt)[offset + 7] << 8) | (uint16_t)(pkt)[offset + 8]),
            rdlength
        );
        offset += 10;

        if (rdlength > 0) {
            if (offset > pkt_len - rdlength) return;
            printf(", rdata: (");
            for (j = 0; j < rdlength; j ++) printf("%02x ", pkt[offset + j]);
            printf(")");
        }
        printf("}");

        offset += rdlength;
    }
}

static void _visualize_dns_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
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
    
    if (!pkt || pkt_len < hdr_len) return;
    
    snprintf(transaction_id, sizeof(transaction_id), "0x%04x", DNS_TRANSACTION_ID(pkt));
    snprintf(qr, sizeof(qr), "%u", (DNS_FLAGS(pkt)) & DNS_QR ? 1 : 0);
    snprintf(opcode, sizeof(opcode), "%u", (DNS_FLAGS(pkt)) & DNS_OPCODE >> 11);
    snprintf(aa, sizeof(aa), "%u", (DNS_FLAGS(pkt)) & DNS_AA ? 1 : 0);
    snprintf(tc, sizeof(tc), "%u", (DNS_FLAGS(pkt)) & DNS_TC ? 1 : 0);
    snprintf(rd, sizeof(rd), "%u", (DNS_FLAGS(pkt)) & DNS_RD ? 1 : 0);
    snprintf(ra, sizeof(ra), "%u", (DNS_FLAGS(pkt)) & DNS_RA ? 1 : 0);
    snprintf(ad, sizeof(ad), "%u", (DNS_FLAGS(pkt)) & DNS_AD ? 1 : 0);
    snprintf(cd, sizeof(cd), "%u", (DNS_FLAGS(pkt)) & DNS_CD ? 1 : 0);
    if (DNS_FLAGS(pkt) & DNS_QR) snprintf(rcode, sizeof(rcode), "%u", DNS_FLAGS(pkt) & DNS_RCODE);
    snprintf(questions, sizeof(questions), "%u", DNS_QUESTIONS(pkt));
    snprintf(answer_rrs, sizeof(answer_rrs), "%u", DNS_ANSWER_RRS(pkt));
    snprintf(auth_rrs, sizeof(auth_rrs), "%u", DNS_AUTH_RRS(pkt));
    snprintf(additional_rrs, sizeof(additional_rrs), "%u", DNS_ADDITIONAL_RRS(pkt));

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
    if (DNS_FLAGS(pkt) & DNS_QR) print_field(DNS_RCODE_LABEL, rcode, 0);
    print_field(DNS_QUESTIONS_LABEL, questions, 0);
    print_field(DNS_ANSWER_RRS_LABEL, answer_rrs, 0);
    print_field(DNS_AUTH_RRS_LABEL, auth_rrs, 0);
    print_field(DNS_ADDITIONAL_RRS_LABEL, additional_rrs, 0);
    end_printing();
}

protocol_info dissect_dns(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < DNS_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_dns_hdr,
        .visualize_protocol_func = _visualize_dns_hdr,
        .hdr_len = 0,
        .encap_protocol = NO_ENCAP_PROTO,
        .encap_proto_table_num = NO_ENCAP_PROTO_TABLE
    };
}