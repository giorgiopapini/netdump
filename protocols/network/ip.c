#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "ip.h"
#include "../../utils/string_utils.h"
#include "../../utils/visualizer.h"
#include "../../utils/lookup_table.h"
#include "../../ipproto.h"
#include "../../utils/formats.h"

lookup_table ipprotos = {
    { IPPROTO_HOPOPTS,      "HOPOP" },
    { IPPROTO_COMP,         "ICMP" },
    { IPPROTO_IGMP,         "IGMP" },
    { IPPROTO_IPV4,         "IPV4" },
    { IPPROTO_TCP,          "TCP" },
    { IPPROTO_EGP,          "EGP" },
    { IPPROTO_PIGP,         "PIGP" },
    { IPPROTO_UDP,          "UDP" },
    { IPPROTO_DCCP,         "DCCP" },
    { IPPROTO_IPV6,         "IGMP" },
    { IPPROTO_ROUTING,      "IPv6 routing" },
    { IPPROTO_FRAGMENT,     "IPv6 fragmentation" },
    { IPPROTO_RSVP,         "RSPV" },
    { IPPROTO_GRE,          "GRE" },
    { IPPROTO_ESP,          "ESP" },
    { IPPROTO_AH,           "AH" },
    { IPPROTO_NHRP,         "NHRP" },
    { IPPROTO_MOBILE,       "MOBILE" },
    { IPPROTO_ICMPV6,       "ICMPV6" },
    { IPPROTO_NONE,         "IPv6 no next header" },
    { IPPROTO_DSTOPTS,      "IPv6 destination options" },
    { IPPROTO_ND,           "ND" },
    { IPPROTO_EIGRP,        "EIGRP" },
    { IPPROTO_OSPF,         "OSPF" },
    { IPPROTO_PIM,          "PIM" },
    { IPPROTO_IPCOMP,       "IPCOMP" },
    { IPPROTO_VRRP,         "VRRP" },
    { IPPROTO_PGM,          "PGM" },
    { IPPROTO_SCTP,         "SCTP" },
    { IPPROTO_MOBILITY,     "MOBILITY" },
    { 0,                    NULL }
};

void ipv4_to_str(char *dest, uint32_t addr) {
    uint8_t bytes[4];
    bytes[3] = addr & 0xFF;
    bytes[2] = (addr >> 8) & 0xFF;
    bytes[1] = (addr >> 16) & 0xFF;
    bytes[0] = (addr >> 24) & 0xFF;
    snprintf(dest, 16, IP_ADDR_FORMAT, IP_TO_STR(bytes));
    dest[IP_ADDR_STR_LEN - 1] = '\0';
}

void print_ipv4(uint32_t addr) {
    char buff[IP_ADDR_STR_LEN];
    ipv4_to_str(buff, addr);
    printf("%s", buff);  
}

void print_ip_hdr(const uint8_t *pkt) {
    char flags[9] = "";  /* max: "DF, MF, \0" */
    ip_hdr *ip_h = (struct ip_hdr *)pkt;
    const char *protocol_name = get_value(ipprotos, ip_h->protocol);

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

    if (NULL != protocol_name) printf(" proto: %s (%d)", protocol_name, ip_h->protocol);
    else printf(" proto: %d", ip_h->protocol);
}

void visualize_ip_hdr(const uint8_t *pkt) {
    ip_hdr *ip_header = (ip_hdr *)pkt;
    const char *encap_proto = get_value(ipprotos, ip_header->protocol);
    encap_proto = NULL == encap_proto ? UNKNOWN : encap_proto;

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
    char protocol[strlen(encap_proto) + 7];  /* proto (xxx)'\0' */
    char checksum[7];  /* 0x0000'\0' are 7 chars */
    char src_addr[IP_ADDR_STR_LEN];
    char dest_addr[IP_ADDR_STR_LEN];

    snprintf(version, sizeof(version), "%d", IP_VERSION(ip_header));
    snprintf(ihl, sizeof(ihl), "%d", IP_HLEN(ip_header));
    snprintf(tos, sizeof(tos), "0x%02x", ip_header->tos);
    snprintf(totlen, sizeof(totlen), "%d", ntohs(ip_header->totlen));
    snprintf(id, sizeof(id), "%d", ntohs(ip_header->identification));
    snprintf(rf, sizeof(rf), "%d", ntohs(ip_header->offset_field) & RF);
    snprintf(df, sizeof(df), "%d", ntohs(ip_header->offset_field) & DF);
    snprintf(mf, sizeof(mf), "%d", ntohs(ip_header->offset_field) & MF);
    uint16_to_bin_str(offset_frag, (ntohs(ip_header->offset_field) & OFFSET_MASK), 14);
    snprintf(ttl, sizeof(ttl), "%d", ip_header->ttl);

    if (NULL != encap_proto) snprintf(protocol, sizeof(protocol), "%s (%d)", encap_proto, ip_header->protocol);
    else snprintf(protocol, sizeof(protocol), "%d", ip_header->protocol);
    
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(ip_header->checksum));
    ipv4_to_str(src_addr, ntohl(ip_header->src_addr));
    ipv4_to_str(dest_addr, ntohl(ip_header->dest_addr));

    start_printing();
    print_hdr_info(IP_HEADER_LABEL, NULL);
    print_field(VERSION_LABEL, version, 0);
    print_field(IHL_LABEL, ihl, 0);
    print_field(TOS_LABEL, tos, 0);
    print_field(TOTLEN_LABEL, totlen, 0);
    print_field(ID_LABEL, id, 0);
    print_field(RF_LABEL, rf, 0);
    print_field(DF_LABEL, df, 0);
    print_field(MF_LABEL, mf, 0);
    print_field(OFFSET_LABEL, offset_frag, 0);
    print_field(TTL_LABEL, ttl, 0);
    print_field(PROTOCOL_LABEL, protocol, 0);
    print_field(CHECKSUM_LABEL, checksum, 0);
    print_field(SRC_ADDR_LABEL, src_addr, 0);
    print_field(DEST_ADDR_LABEL, dest_addr, 0);
    end_printing();
}