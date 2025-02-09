#include "ip_protos.h"

#include "network/ip.h"
#include "network/icmp.h"
#include "network/ipv6.h"
#include "network/icmpv6.h"
#include "transport/tcp.h"
#include "transport/udp.h"

protocol_handler ip_protos[] = {
    { IPPROTO_HOPOPTS,      PROTOCOL_LAYER_NONE, NULL, "HOPOP" },
    { IPPROTO_ICMP,         PROTOCOL_LAYER_NETWORK, dissect_icmp, "ICMP" },
    { IPPROTO_IGMP,         PROTOCOL_LAYER_NETWORK, NULL, "IGMP" },
    { IPPROTO_IPV4,         PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4" },
    { IPPROTO_TCP,          PROTOCOL_LAYER_TRANSPORT, dissect_tcp, "TCP" },
    { IPPROTO_EGP,          PROTOCOL_LAYER_NONE, NULL, "EGP" },
    { IPPROTO_PIGP,         PROTOCOL_LAYER_NONE, NULL, "PIGP" },
    { IPPROTO_UDP,          PROTOCOL_LAYER_TRANSPORT, dissect_udp, "UDP" },
    { IPPROTO_DCCP,         PROTOCOL_LAYER_NONE, NULL, "DCCP" },
    { IPPROTO_IPV6,         PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6" },
    { IPPROTO_ROUTING,      PROTOCOL_LAYER_NONE, NULL, "IPv6 routing" },
    { IPPROTO_FRAGMENT,     PROTOCOL_LAYER_NONE, NULL, "IPv6 fragmentation" },
    { IPPROTO_RSVP,         PROTOCOL_LAYER_NONE, NULL, "RSPV" },
    { IPPROTO_GRE,          PROTOCOL_LAYER_NONE, NULL, "GRE" },
    { IPPROTO_ESP,          PROTOCOL_LAYER_NONE, NULL, "ESP" },
    { IPPROTO_AH,           PROTOCOL_LAYER_NONE, NULL, "AH" },
    { IPPROTO_NHRP,         PROTOCOL_LAYER_NONE, NULL, "NHRP" },
    { IPPROTO_MOBILE,       PROTOCOL_LAYER_NONE, NULL, "MOBILE" },
    { IPPROTO_ICMPV6,       PROTOCOL_LAYER_NETWORK, dissect_icmpv6, "ICMPv6" },
    { IPPROTO_NONE,         PROTOCOL_LAYER_NONE, NULL, "IPv6 no next header" },
    { IPPROTO_DSTOPTS,      PROTOCOL_LAYER_NONE, NULL, "IPv6 destination options" },
    { IPPROTO_ND,           PROTOCOL_LAYER_NONE, NULL, "ND" },
    { IPPROTO_EIGRP,        PROTOCOL_LAYER_NONE, NULL, "EIGRP" },
    { IPPROTO_OSPF,         PROTOCOL_LAYER_NONE, NULL, "OSPF" },
    { IPPROTO_PIM,          PROTOCOL_LAYER_NONE, NULL, "PIM" },
    { IPPROTO_IPCOMP,       PROTOCOL_LAYER_NONE, NULL, "IPCOMP" },
    { IPPROTO_VRRP,         PROTOCOL_LAYER_NONE, NULL, "VRRP" },
    { IPPROTO_PGM,          PROTOCOL_LAYER_NONE, NULL, "PGM" },
    { IPPROTO_SCTP,         PROTOCOL_LAYER_NONE, NULL, "SCTP" },
    { IPPROTO_MOBILITY,     PROTOCOL_LAYER_NONE, NULL, "MOBILITY" },
    { 0,                    PROTOCOL_LAYER_NONE, NULL, NULL }
};