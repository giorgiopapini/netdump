#include "ip_protos.h"

#include <stddef.h>

#include "network/ip.h"
#include "network/icmp.h"
#include "network/ipv6.h"
#include "network/icmpv6.h"
#include "transport/tcp.h"
#include "transport/udp.h"


hashmap *ip_protos = NULL;

void load_ip_protos(void) {
    ip_protos = create_hashmap(IP_PROTOS_BUCKETS_NUM);
    ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_ICMP, PROTOCOL_LAYER_NETWORK, dissect_icmp, "ICMP");
    ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_IPV4, PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4");
    ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_TCP, PROTOCOL_LAYER_TRANSPORT, dissect_tcp, "TCP");
    ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_UDP, PROTOCOL_LAYER_TRANSPORT, dissect_udp, "UDP");
    ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_IPV6, PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6");
    ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_ICMPV6, PROTOCOL_LAYER_NETWORK, dissect_icmpv6, "ICMPv6");
}