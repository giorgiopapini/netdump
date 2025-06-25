#include "dhcp.h"

#include <stdio.h>

#include "../../utils/formats.h"
#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static void _print_options(const uint8_t *pkt, size_t len);
static void _print_dhcp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_dhcp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _print_options(const uint8_t *pkt, size_t len) {
    size_t i;

    printf(", options: [");
    for (i = DHCP_OPTIONS_OFFSET; i < len && DHCP_END_OPTIONS != (const uint8_t)pkt[i]; i ++) {
        printf("%02x ", pkt[i]);
    }
    printf("%02x]", pkt[i]);
}

static void _print_dhcp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    if (!pkt || pkt_len < hdr_len) return;

    printf("op: %u", DHCP_OP(pkt));
    if (1 == DHCP_OP(pkt)) printf(" (BOOTREQUEST)");
    else if (2 == DHCP_OP(pkt)) printf(" (BOOTREPLY)");

    printf(
        ", htype: 0x%02x, hlen: %u, hops: %u, xid: 0x%08x, elapsed: %u",
        DHCP_HTYPE(pkt),
        DHCP_HLEN(pkt),
        DHCP_HOPS(pkt),
        DHCP_XID(pkt),
        DHCP_SECS(pkt)
    );
    
    if (DHCP_FLAGS(pkt) & DHCP_BROADCAST_MASK) printf(", broadcast");
    else printf(", unicast");

    printf(", client_ip: ");
    print_ipv4(DHCP_CIADDR(pkt));
    printf(", your_(client)_ip: ");
    print_ipv4(DHCP_YIADDR(pkt));
    printf(", next_server_ip: ");
    print_ipv4(DHCP_SIADDR(pkt));
    printf(", relay_agent_ip: ");
    print_ipv4(DHCP_GIADDR(pkt));
    printf(", client_mac: ");
    print_mac(DHCP_CHADDR(pkt));

    printf(", server_hostname: %s", DHCP_SNAME(pkt)[0] ? (const uint8_t *)DHCP_SNAME(pkt) : (const uint8_t *)"(none)");
    printf(", boot_file: %s", DHCP_BOOTFILE(pkt)[0] ? (const uint8_t *)DHCP_BOOTFILE(pkt) : (const uint8_t *)"(none)");
    printf(", magic_cookie: 0x%08x", DHCP_MAGIC_COOKIE(pkt));
    _print_options(pkt, pkt_len);
}

static void _visualize_dhcp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char operation[5];
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
    
    if (!pkt || pkt_len < hdr_len) return;

    snprintf(operation, sizeof(operation), "%u", DHCP_OP(pkt));
    snprintf(htype, sizeof(htype), "0x%02x", DHCP_HTYPE(pkt));
    snprintf(hlen, sizeof(hlen), "%u", DHCP_HLEN(pkt));
    snprintf(hops, sizeof(hops), "%u", DHCP_HOPS(pkt));
    snprintf(xid, sizeof(xid), "0x%08x", DHCP_XID(pkt));
    snprintf(secs, sizeof(secs), "%u", DHCP_SECS(pkt));
    snprintf(ciaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(DHCP_CIADDR(pkt)));
    snprintf(yiaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(DHCP_YIADDR(pkt)));
    snprintf(siaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(DHCP_SIADDR(pkt)));
    snprintf(giaddr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(DHCP_GIADDR(pkt)));
    snprintf(chaddr, sizeof(chaddr), MAC_ADDR_FORMAT, MAC_TO_STR(DHCP_CHADDR(pkt)));
    snprintf(sname, sizeof(sname), "%s", DHCP_SNAME(pkt)[0] ? (const uint8_t *)DHCP_SNAME(pkt) : (const uint8_t *)"(none)");
    snprintf(boot_file, sizeof(boot_file), "%s", DHCP_BOOTFILE(pkt)[0] ? (const uint8_t *)DHCP_BOOTFILE(pkt) : (const uint8_t *)"(none)");
    snprintf(magic_cookie, sizeof(magic_cookie), "0x%08x", DHCP_MAGIC_COOKIE(pkt));

    start_printing();
    print_additional_info("Options fields not represented in ascii art");
    print_field(DHCP_OP_LABEL, operation, 0);
    print_field(DHCP_HTYPE_LABEL, htype, 0);
    print_field(DHCP_HLEN_LABEL, hlen, 0);
    print_field(DHCP_HOPS_LABEL, hops, 0);
    print_field(DHCP_XID_LABEL, xid, 0);
    print_field(DHCP_SECS_LABEL, secs, 0);

    //if (DHCP_FLAGS(pkt) & DHCP_BROADCAST_MASK) print_field(DHCP_BOOTP_FLAGS_LABEL, "broadcast", 0);
    //else print_field(DHCP_BOOTP_FLAGS_LABEL, "unicast", 0);

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

protocol_info dissect_dhcp(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    if (!pkt || pkt_len < MIN_DHCP_HDR_LEN) return NO_ENCAP_PROTO;
    
    SHOW_OUTPUT(pkt, pkt_len, MIN_DHCP_HDR_LEN, fmt, _print_dhcp_hdr, _visualize_dhcp_hdr);
    return NO_ENCAP_PROTO;
}