#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "ip.h"

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
    printf("visualize ip hdr\n");
}