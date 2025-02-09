#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "ip.h"
#include "../ip_protos.h"
#include "../../utils/string_utils.h"
#include "../../utils/visualizer.h"
#include "../../utils/formats.h"


/*
void print_ip_options(const uint8_t *pkt) {

}
*/

void print_ip_hdr(const uint8_t *pkt) {
    char flags[9] = "";  /* max: "DF, MF, \0" */

    /* ===================== printing src (IP) > dest (IP) ====================== */
    print_ipv4(ntohl(IP_SRC_ADDR(pkt)));
    printf(" > ");
    print_ipv4(ntohl(IP_DEST_ADDR(pkt)));
    /* ========================================================================== */

    printf(
        " tos: 0x%01x, ttl: %d, id: %d, offset: %d,", 
        IP_TOS(pkt), 
        IP_TTL(pkt), 
        ntohs(IP_ID(pkt)),
        ntohs(IP_OFFSET(pkt)) & IP_OFFSET_MASK
    );

    /* ============================= printing flags ============================= */
    printf(" flags: [");
    if (ntohs(IP_OFFSET(pkt)) & IP_DF) strcat(flags, "DF, ");
    if (ntohs(IP_OFFSET(pkt)) & IP_MF) strcat(flags, "MF, ");
    flags[strlen(flags) - 2] = '\0';    /* remove last ", " chars */
    printf("%s],", flags);
    /* ========================================================================== */

    printf(" proto: %d", IP_PROTOCOL(pkt));
    
    /*if (IP_HLEN(pkt) > 5) print_ip_options(pkt);*/
}

void visualize_ip_hdr(const uint8_t *pkt) {
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

    snprintf(version, sizeof(version), "%u", IP_VERSION(pkt));
    snprintf(ihl, sizeof(ihl), "%u", IP_HLEN(pkt));
    snprintf(tos, sizeof(tos), "0x%02x", IP_TOS(pkt));
    snprintf(totlen, sizeof(totlen), "%u", ntohs(IP_TOTLEN(pkt)));
    snprintf(id, sizeof(id), "%u", ntohs(IP_ID(pkt)));
    snprintf(rf, sizeof(rf), "%u", (ntohs(IP_OFFSET(pkt)) & IP_RF) ? 1 : 0);
    snprintf(df, sizeof(df), "%u", (ntohs(IP_OFFSET(pkt)) & IP_DF) ? 1 : 0);
    snprintf(mf, sizeof(mf), "%u", (ntohs(IP_OFFSET(pkt)) & IP_MF) ? 1 : 0);
    uint_to_bin_str(offset_frag, (ntohs(IP_OFFSET(pkt)) & IP_OFFSET_MASK), sizeof(offset_frag));
    snprintf(ttl, sizeof(ttl), "%u", IP_TTL(pkt));
    snprintf(protocol, sizeof(protocol), "%u", IP_PROTOCOL(pkt));
    
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(IP_CHECKSUM(pkt)));
    snprintf(src_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(IP_SRC_ADDR(pkt))));
    snprintf(dest_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(IP_DEST_ADDR(pkt))));

    start_printing();
    print_hdr_info(IP_HEADER_LABEL, "Options fields not represented in ascii art");
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

protocol_info dissect_ip(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_ip_hdr, visualize_ip_hdr);
    return (protocol_info){ .protocol = IP_PROTOCOL(pkt), .offset = (IP_HLEN(pkt) * 4), .table = ip_protos };
}