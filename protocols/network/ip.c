#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "ip.h"
#include "../../utils/string_utils.h"
#include "../../utils/visualizer.h"

void print_ipv4(uint32_t addr) {
    uint8_t bytes[4];
    bytes[0] = addr & 0xFF;
    bytes[1] = (addr >> 8) & 0xFF;
    bytes[2] = (addr >> 16) & 0xFF;
    bytes[3] = (addr >> 24) & 0xFF;   
    printf("%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);  
}

void print_ip_hdr(const uint8_t *pkt) {
    char flags[9] = "";  /* max: "DF, MF, \0" */
    ip_hdr *ip_h = (struct ip_hdr *)pkt;

    /* ===================== printing src (IP) > dest (IP) ====================== */
    print_ipv4(ntohl(ip_h->src_addr));
    printf(" > ");
    print_ipv4(ntohl(ip_h->dest_addr));
    /* ========================================================================== */

    printf(
        " tos: 0x%01x, ttl: %d, id: %d, offset: %d,", 
        ip_h->tos, 
        ip_h->ttl, 
        ntohs(ip_h->identification),
        ntohs(ip_h->offset_field) & OFFSET_MASK
    );

    /* ============================= printing flags ============================= */
    printf(" flags: [");
    if (ntohs(ip_h->offset_field) & DF) strcat(flags, "DF, ");
    if (ntohs(ip_h->offset_field) & MF) strcat(flags, "MF, ");
    flags[strlen(flags) - 2] = '\0';    /* remove last ", " chars */
    printf("%s],", flags);
    /* ========================================================================== */

    printf(" proto: %s (%d)", "ENCAP_PROTO_NAME", ip_h->protocol);  /* SET ENCAP_PROTO_NAME, create lookup table somewhere */
}

void visualize_ip_hdr(const uint8_t *pkt) {
    ip_hdr *ip_header = (ip_hdr *)pkt;
    char version[2];
    char ihl[3];
    char tos[5];
    char totlen[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    char id[6];
    char mf[2];
    char df[2];

    snprintf(version, sizeof(version), "%d", IP_VERSION(ip_header->vhlen));
    snprintf(ihl, sizeof(ihl), "%d", IP_HLEN(ip_header->vhlen));
    snprintf(tos, sizeof(tos), "0x%04x", ip_header->tos);
    snprintf(totlen, sizeof(totlen), "%d", ntohs(ip_header->totlen));
    snprintf(id, sizeof(id), "%d", ntohs(ip_header->identification));
    snprintf(mf, sizeof(mf), "%d", ntohs(ip_header->offset_field) & MF);
    snprintf(df, sizeof(df), "%d", ntohs(ip_header->offset_field) & DF);

    start_printing();
    print_hdr_info(IP_HEADER_LABEL, NULL);
    print_field(VERSION_LABEL, version, 0);
    print_field(IHL_LABEL, ihl, 0);
    print_field(TOS_LABEL, tos, 0);
    print_field(TOTLEN_LABEL, totlen, 0);
    print_field(ID_LABEL, id, 0);
    print_field(DF_LABEL, df, 0);
    print_field(MF_LABEL, mf, 0);
    end_printing();
}