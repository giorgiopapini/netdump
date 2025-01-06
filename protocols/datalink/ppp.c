#include <stdio.h>
#include <arpa/inet.h>

#include "ppp.h"
#include "../../ppp_protos.h"
#include "../../utils/lookup_table.h"
#include "../../utils/visualizer.h"

lookup_table ppp_protos = {
    { PPP_IP,           "IPv4" },
    { PPP_OSI,          "OSI Network Layer" },
    { PPP_NS,           "Xerox NS IDP" },
    { PPP_DECNET,       "DECnet Phase IV" },
    { PPP_APPLE,        "Appletalk" },
    { PPP_IPX,          "Novell IPX" },
    { PPP_VJC,          "Van Jacobson Compressed TCP/IP" },
    { PPP_VJNC,         "Van Jacobson Uncompressed TCP/IP" },
    { PPP_BRPDU,        "Bridging PDU" },
    { PPP_STII,         "Stream Protocol (ST-II)" },
    { PPP_VINES,        "Banyan Vines" },
    { PPP_ML,           "Multi-Link PPP" },
    { PPP_IPV6,         "IPv6" },
    { PPP_COMP,         "Compressed Datagram" },
    { PPP_HELLO,        "802.1d Hello Packets" },
    { PPP_LUXCOM,       "Luxcom" },
    { PPP_SNS,          "Sigma Network Systems" },
    { PPP_MPLS_UCAST,   "MPLS Unicast (RFC 3032)" },
    { PPP_MPLS_MCAST,   "MPLS Multicast (RFC 3022)" },
    { PPP_IPCP,         "IP Control Protocol" },
    { PPP_OSICP,        "OSI Control Protocol" },
    { PPP_NSCP,         "Xerox NS IDP Control Protocol" },
    { PPP_DECNETCP,     "DECnet Control Protocol" },
    { PPP_APPLECP,      "Appletalk Control Protocol" },
    { PPP_IPXCP,        "Novell IPX Control Protocol" },
    { PPP_STIICP,       "Stream Protocol Control Protocol" },
    { PPP_VINESCP,      "Banyan Vines Control Protocol" },
    { PPP_IPV6CP,       "IPv6 Control Protocol" },
    { PPP_CCP,          "Compress Control Protocol" },
    { PPP_MPLSCP,       "MPLS Control Protocol (RFC 3022)" },
    { PPP_LCP,          "Link Control Protocol" },
    { PPP_PAP,          "Password Authentication Protocol" },
    { PPP_LQM,          "Link Quality Monitoring" },
    { PPP_SPAP,         "Simple Password Authentication Protocol" },
    { PPP_CHAP,         "Challenge Handshake Authentication Protocol" },
    { PPP_BACP,         "Bandwidth Allocation Control Protocol" },
    { PPP_BAP,          "Bandwidth Allocation Protocol" },
    { PPP_MPCP,         "Multi-Link Control Protocol" },
    { PPP_SPAP_OLD,     "Old Simple Password Authentication Protocol" },
    { PPP_EAP,          "Extensible Authentication Protocol" },
    { 0,                NULL }
};


size_t ppp_hdr_len(const uint8_t *pkt) { return PPP_LEN; }


void print_ppp_hdr(const uint8_t *pkt) {
    const char *protocol_name = get_value(ppp_protos, ntohs(PROTOCOL(pkt)));
    printf(
        "addr: 0x%02x, control: 0x%02x, protocol: 0x%04x (%s)",
        ADDRESS(pkt),
        CONTROL(pkt),
        ntohs(PROTOCOL(pkt)),
        protocol_name
    );
}

void visualize_ppp_hdr(const uint8_t *pkt) {
    char address[5];  /* 0x00'\0' are 5 chars */
    char control[5];
    char protocol[7];  /* 0x0000'\0' are 7 chars */

    snprintf(address, sizeof(address), "0x%02x", ADDRESS(pkt));
    snprintf(control, sizeof(control), "0x%02x", CONTROL(pkt));
    snprintf(protocol, sizeof(protocol), "0x%04x", ntohs(PROTOCOL(pkt)));

    start_printing();
    print_hdr_info(PPP_HEADER_LABEL, NULL);
    print_field(ADDRESS_LABEL, address, 0);
    print_field(CONTROL_LABEL, control, 0);
    print_field(PROTOCOL_LABEL, protocol, 0);
    end_printing();
}
