#include <stdio.h>
#include <string.h>

#include "ip.h"
#include "../../utils/string_utils.h"
#include "../../utils/visualizer.h"
#include "../../utils/formats.h"
#include "../proto_tables_nums.h"


/*
void print_ip_options(const uint8_t *pkt) {

}
*/

void print_ip_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_ip_hdr(const uint8_t *pkt, size_t pkt_len);

void print_ip_hdr(const uint8_t *pkt, size_t pkt_len) {
    char flags[9] = "";  /* max: "DF, MF, \0" */
    (void)pkt_len;

    /* ===================== printing src (IP) > dest (IP) ====================== */
    print_ipv4(NP_IP_SRC_ADDR(pkt));
    printf(" > ");
    print_ipv4(NP_IP_DEST_ADDR(pkt));
    /* ========================================================================== */

    printf(
        " tos: 0x%01x, ttl: %d, id: %d, offset: %d,", 
        NP_IP_TOS(pkt), 
        NP_IP_TTL(pkt), 
        NP_IP_ID(pkt),
        NP_IP_OFFSET(pkt) & NP_IP_OFFSET_MASK
    );

    /* ============================= printing flags ============================= */
    printf(" flags: [");
    if (NP_IP_OFFSET(pkt) & NP_IP_DF) strcat(flags, "DF, ");
    if (NP_IP_OFFSET(pkt) & NP_IP_MF) strcat(flags, "MF, ");
    flags[strlen(flags) - 2] = '\0';    /* remove last ", " chars */
    printf("%s],", flags);
    /* ========================================================================== */

    printf(" proto: %d", NP_IP_PROTOCOL(pkt));
    
    /*if (IP_HLEN(pkt) > 5) print_ip_options(pkt);*/
}

void visualize_ip_hdr(const uint8_t *pkt, size_t pkt_len) {
    char version[4];
    char ihl[4];
    char tos[5];  /* 0x00'\0' are 5 chars */
    char totlen[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    char id[6];
    char rf[2];
    char df[2];
    char mf[2];
    char offset_frag[14];
    char ttl[4];
    char protocol[4];  /* xxx'\0' */
    char checksum[7];  /* 0x0000'\0' are 7 chars */
    char src_addr[IP_ADDR_STR_LEN];
    char dest_addr[IP_ADDR_STR_LEN];
    (void)pkt_len;
    
    snprintf(version, sizeof(version), "%u", NP_IP_VERSION(pkt));
    snprintf(ihl, sizeof(ihl), "%u", NP_IP_HLEN(pkt));
    snprintf(tos, sizeof(tos), "0x%02x", NP_IP_TOS(pkt));
    snprintf(totlen, sizeof(totlen), "%u", NP_IP_TOTLEN(pkt));
    snprintf(id, sizeof(id), "%u", NP_IP_ID(pkt));
    snprintf(rf, sizeof(rf), "%u", (NP_IP_OFFSET(pkt) & NP_IP_RF) ? 1 : 0);
    snprintf(df, sizeof(df), "%u", (NP_IP_OFFSET(pkt) & NP_IP_DF) ? 1 : 0);
    snprintf(mf, sizeof(mf), "%u", (NP_IP_OFFSET(pkt) & NP_IP_MF) ? 1 : 0);
    uint_to_bin_str(offset_frag, (NP_IP_OFFSET(pkt) & NP_IP_OFFSET_MASK), sizeof(offset_frag));
    snprintf(ttl, sizeof(ttl), "%u", NP_IP_TTL(pkt));
    snprintf(protocol, sizeof(protocol), "%u", NP_IP_PROTOCOL(pkt));
    
    snprintf(checksum, sizeof(checksum), "0x%04x", NP_IP_CHECKSUM(pkt));
    snprintf(src_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(NP_IP_SRC_ADDR(pkt)));
    snprintf(dest_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(NP_IP_DEST_ADDR(pkt)));

    start_printing();
    print_additional_info("Options fields not represented in ascii art");
    print_field(IP_VERSION_LABEL, version, 0);
    print_field(IP_IHL_LABEL, ihl, 0);
    print_field(IP_TOS_LABEL, tos, 0);
    print_field(IP_TOTLEN_LABEL, totlen, 0);
    print_field(IP_ID_LABEL, id, 0);
    print_field(IP_RF_LABEL, rf, 0);
    print_field(IP_DF_LABEL, df, 0);
    print_field(IP_MF_LABEL, mf, 0);
    print_field(IP_OFFSET_LABEL, offset_frag, 0);
    print_field(IP_TTL_LABEL, ttl, 0);
    print_field(IP_PROTOCOL_LABEL, protocol, 0);
    print_field(IP_CHECKSUM_LABEL, checksum, 0);
    print_field(IP_SRC_ADDR_LABEL, src_addr, 0);
    print_field(IP_DEST_ADDR_LABEL, dest_addr, 0);
    end_printing();
}

protocol_info dissect_ip(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_ip_hdr, visualize_ip_hdr);
    return (protocol_info){ .protocol = NP_IP_PROTOCOL(pkt), .offset = (NP_IP_HLEN(pkt) * 4), .proto_table_num = IP_PROTOS };
}