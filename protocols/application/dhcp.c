#include <stdio.h>
#include <arpa/inet.h>

#include "dhcp.h"
#include "../../utils/visualizer.h"
#include "../../utils/formats.h"


void print_options(const uint8_t *pkt, uint32_t len) {
    int i;

    printf(", options: [");
    for (i = DHCP_OPTIONS_OFFSET; i < len && DHCP_END_OPTIONS != (uint8_t)pkt[i]; i ++) {
        printf("%02x ", pkt[i]);
    }
    printf("%02x]", pkt[i]);
}

void print_dhcp_hdr(const uint8_t *pkt, uint32_t len) {
    uint8_t sname[64];
    uint8_t file[128];
    uint32_t magic;
    uint8_t options[312];

    printf("op: %u", DHCP_OP(pkt));

    if (1 == DHCP_OP(pkt)) printf(" (BOOTREQUEST)");
    else if (2 == DHCP_OP(pkt)) printf(" (BOOTREPLY)");

    printf(
        ", htype: 0x%02x, hlen: %u, hops: %u, xid: 0x%08x, elapsed: %u",
        DHCP_HTYPE(pkt),
        DHCP_HLEN(pkt),
        DHCP_HOPS(pkt),
        ntohl(DHCP_XID(pkt)),
        ntohs(DHCP_SECS(pkt))
    );
    
    if (ntohs(DHCP_FLAGS(pkt)) & DHCP_BROADCAST_MASK) printf(", broadcast");
    else printf(", unicast");

    printf(", client_ip: ");
    print_ipv4(ntohl(DHCP_CIADDR(pkt)));
    printf(", your_(client)_ip: ");
    print_ipv4(ntohl(DHCP_YIADDR(pkt)));
    printf(", next_server_ip: ");
    print_ipv4(ntohl(DHCP_SIADDR(pkt)));
    printf(", relay_agent_ip: ");
    print_ipv4(ntohl(DHCP_GIADDR(pkt)));
    printf(", client_mac: ");
    print_mac(DHCP_CHADDR(pkt));

    printf(", server_hostname: %s", DHCP_SNAME(pkt)[0] ? (uint8_t *)DHCP_SNAME(pkt) : (uint8_t *)"(none)");
    printf(", boot_file: %s", DHCP_BOOTFILE(pkt)[0] ? (uint8_t *)DHCP_BOOTFILE(pkt) : (uint8_t *)"(none)");
    printf(", magic_cookie: 0x%08x", ntohl(DHCP_MAGIC_COOKIE(pkt)));
    print_options(pkt, len);
}

void visualize_dhcp_hdr(const uint8_t *pkt, uint32_t len) {
    char operation[2];
    char htype[5];  /* 0x00'\0' */
    char hlen[4];
    char hops[4];
    char xid[11];
    char secs[6];
    char ciaddr[IP_ADDR_STR_LEN];
    char yiaddr[IP_ADDR_STR_LEN];
    char siaddr[IP_ADDR_STR_LEN];
    char giaddr[IP_ADDR_STR_LEN];
    char chaddr[MAC_ADDR_STR_LEN];
    char sname[65];
    char boot_file[129];
    char magic_cookie[11];

    snprintf(operation, sizeof(operation), "%u", DHCP_OP(pkt));
    snprintf(htype, sizeof(htype), "0x%02x", DHCP_HTYPE(pkt));
    snprintf(hlen, sizeof(hlen), "%u", DHCP_HLEN(pkt));
    snprintf(hops, sizeof(hops), "%u", DHCP_HOPS(pkt));
    snprintf(xid, sizeof(xid), "0x%08x", ntohl(DHCP_XID(pkt)));
    snprintf(secs, sizeof(secs), "%u", ntohs(DHCP_SECS(pkt)));
    snprintf(ciaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(DHCP_CIADDR(pkt))));
    snprintf(yiaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(DHCP_YIADDR(pkt))));
    snprintf(siaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(DHCP_SIADDR(pkt))));
    snprintf(giaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(DHCP_GIADDR(pkt))));
    snprintf(chaddr, sizeof(chaddr), MAC_ADDR_FORMAT, MAC_TO_STR(DHCP_CHADDR(pkt)));
    snprintf(sname, sizeof(sname), "%s", DHCP_SNAME(pkt)[0] ? (uint8_t *)DHCP_SNAME(pkt) : (uint8_t *)"(none)");
    snprintf(boot_file, sizeof(boot_file), "%s", DHCP_BOOTFILE(pkt)[0] ? (uint8_t *)DHCP_BOOTFILE(pkt) : (uint8_t *)"(none)");
    snprintf(magic_cookie, sizeof(magic_cookie), "0x%08x", ntohl(DHCP_MAGIC_COOKIE(pkt)));

    start_printing();
    print_additional_info("Options fields not represented in ascii art");
    print_field(DHCP_OP_LABEL, operation, 0);
    print_field(DHCP_HTYPE_LABEL, htype, 0);
    print_field(DHCP_HLEN_LABEL, hlen, 0);
    print_field(DHCP_HOPS_LABEL, hops, 0);
    print_field(DHCP_XID_LABEL, xid, 0);
    print_field(DHCP_SECS_LABEL, secs, 0);

    if (ntohs(DHCP_FLAGS(pkt)) & DHCP_BROADCAST_MASK) print_field(DHCP_BOOTP_FLAGS_LABEL, "broadcast", 0);
    else print_field(DHCP_BOOTP_FLAGS_LABEL, "unicast", 0);

    print_field(DHCP_CIADDR_LABEL, ciaddr, 0);
    print_field(DHCP_YIADDR_LABEL, yiaddr, 0);
    print_field(DHCP_SIADDR_LABEL, siaddr, 0);
    print_field(DHCP_GIADDR_LABEL, giaddr, 0);
    print_field(DHCP_CHADDR_LABEL, chaddr, 0);
    print_field(DHCP_SNAME_LABEL, sname, 0);
    print_field(DHCP_BOOTFILE_LABEL, boot_file, 0);
    print_field(DHCP_MAGIC_COOKIE_LABEL, magic_cookie, 0);
    end_printing();
}

protocol_info dissect_dhcp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_dhcp_hdr, visualize_dhcp_hdr);
    return NO_ENCAP_PROTO;
}