#include "ethertypes.h"

#include <stddef.h>

#include "datalink/ppp.h"
#include "datalink/vlan.h"
#include "network/ip.h"
#include "network/ipv6.h"
#include "network/arp.h"


hashmap *ethertypes = NULL;

void load_ethertypes() {
    ethertypes = create_hashmap(ETHERTYPES_BUCKETS_NUM);
    ADD_PROTO_HANDLER_ENTRY(ethertypes, ETHERTYPE_IP, PROTOCOL_LAYER_DATALINK, dissect_ip, "IPv4");
    ADD_PROTO_HANDLER_ENTRY(ethertypes, ETHERTYPE_IPV6, PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6");
    ADD_PROTO_HANDLER_ENTRY(ethertypes, ETHERTYPE_8021Q, PROTOCOL_LAYER_DATALINK, dissect_vlan, "802.1Q \"VLAN\"");
    ADD_PROTO_HANDLER_ENTRY(ethertypes, ETHERTYPE_ARP, PROTOCOL_LAYER_NETWORK, dissect_arp, "ARP");
    ADD_PROTO_HANDLER_ENTRY(ethertypes, ETHERTYPE_RARP, PROTOCOL_LAYER_NETWORK, dissect_arp, "Reverse ARP");
    ADD_PROTO_HANDLER_ENTRY(ethertypes, ETHERTYPE_PPP, PROTOCOL_LAYER_DATALINK, dissect_ppp, "PPP");
}