#include "ppp_protos.h"

#include <stddef.h>

#include "../utils/protocol.h"
#include "network/ip.h"
#include "network/ipv6.h"
#include "network/icmpv6.h"

protocol_handler ppp_protos[] = {
    { PPP_IP,           PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4" },
    { PPP_OSI,          PROTOCOL_LAYER_NONE, NULL, "OSI Network Layer" },
    { PPP_NS,           PROTOCOL_LAYER_NONE, NULL, "Xerox NS IDP" },
    { PPP_DECNET,       PROTOCOL_LAYER_NONE, NULL, "DECnet Phase IV" },
    { PPP_APPLE,        PROTOCOL_LAYER_NONE, NULL, "Appletalk" },
    { PPP_IPX,          PROTOCOL_LAYER_NONE, NULL, "Novell IPX" },
    { PPP_VJC,          PROTOCOL_LAYER_NONE, NULL, "Van Jacobson Compressed TCP/IP" },
    { PPP_VJNC,         PROTOCOL_LAYER_NONE, NULL, "Van Jacobson Uncompressed TCP/IP" },
    { PPP_BRPDU,        PROTOCOL_LAYER_NONE, NULL, "Bridging PDU" },
    { PPP_STII,         PROTOCOL_LAYER_NONE, NULL, "Stream Protocol (ST-II)" },
    { PPP_VINES,        PROTOCOL_LAYER_NONE, NULL, "Banyan Vines" },
    { PPP_ML,           PROTOCOL_LAYER_NONE, NULL, "Multi-Link PPP" },
    { PPP_IPV6,         PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6" },
    { PPP_COMP,         PROTOCOL_LAYER_NONE, NULL, "Compressed Datagram" },
    { PPP_HELLO,        PROTOCOL_LAYER_NONE, NULL, "802.1d Hello Packets" },
    { PPP_LUXCOM,       PROTOCOL_LAYER_NONE, NULL, "Luxcom" },
    { PPP_SNS,          PROTOCOL_LAYER_NONE, NULL, "Sigma Network Systems" },
    { PPP_MPLS_UCAST,   PROTOCOL_LAYER_NONE, NULL, "MPLS Unicast (RFC 3032)" },
    { PPP_MPLS_MCAST,   PROTOCOL_LAYER_NONE, NULL, "MPLS Multicast (RFC 3022)" },
    { PPP_IPCP,         PROTOCOL_LAYER_NONE, NULL, "IP Control Protocol" },
    { PPP_OSICP,        PROTOCOL_LAYER_NONE, NULL, "OSI Control Protocol" },
    { PPP_NSCP,         PROTOCOL_LAYER_NONE, NULL, "Xerox NS IDP Control Protocol" },
    { PPP_DECNETCP,     PROTOCOL_LAYER_NONE, NULL, "DECnet Control Protocol" },
    { PPP_APPLECP,      PROTOCOL_LAYER_NONE, NULL, "Appletalk Control Protocol" },
    { PPP_IPXCP,        PROTOCOL_LAYER_NONE, NULL, "Novell IPX Control Protocol" },
    { PPP_STIICP,       PROTOCOL_LAYER_NONE, NULL, "Stream Protocol Control Protocol" },
    { PPP_VINESCP,      PROTOCOL_LAYER_NONE, NULL, "Banyan Vines Control Protocol" },
    { PPP_IPV6CP,       PROTOCOL_LAYER_NETWORK, dissect_icmpv6, "IPv6 Control Protocol" },
    { PPP_CCP,          PROTOCOL_LAYER_NONE, NULL, "Compress Control Protocol" },
    { PPP_MPLSCP,       PROTOCOL_LAYER_NONE, NULL, "MPLS Control Protocol (RFC 3022)" },
    { PPP_LCP,          PROTOCOL_LAYER_NONE, NULL, "Link Control Protocol" },
    { PPP_PAP,          PROTOCOL_LAYER_NONE, NULL, "Password Authentication Protocol" },
    { PPP_LQM,          PROTOCOL_LAYER_NONE, NULL, "Link Quality Monitoring" },
    { PPP_SPAP,         PROTOCOL_LAYER_NONE, NULL, "Simple Password Authentication Protocol" },
    { PPP_CHAP,         PROTOCOL_LAYER_NONE, NULL, "Challenge Handshake Authentication Protocol" },
    { PPP_BACP,         PROTOCOL_LAYER_NONE, NULL, "Bandwidth Allocation Control Protocol" },
    { PPP_BAP,          PROTOCOL_LAYER_NONE, NULL, "Bandwidth Allocation Protocol" },
    { PPP_MPCP,         PROTOCOL_LAYER_NONE, NULL, "Multi-Link Control Protocol" },
    { PPP_SPAP_OLD,     PROTOCOL_LAYER_NONE, NULL, "Old Simple Password Authentication Protocol" },
    { PPP_EAP,          PROTOCOL_LAYER_NONE, NULL, "Extensible Authentication Protocol" },
    NULL_PROTO_HANDLER
};