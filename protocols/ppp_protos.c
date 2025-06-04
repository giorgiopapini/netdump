#include "ppp_protos.h"

#include <stddef.h>

#include "../utils/hashmap.h"
#include "../utils/protocol.h"
#include "network/ip.h"
#include "network/ipv6.h"
#include "network/icmpv6.h"


hashmap *ppp_protos = NULL;

void load_ppp_protos(void) {
    ppp_protos = create_hashmap(PPP_PROTOS_BUCKETS_NUM);
    ADD_PROTO_HANDLER_ENTRY(ppp_protos, PPP_IP, PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4");
    ADD_PROTO_HANDLER_ENTRY(ppp_protos, PPP_IPV6, PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6");
    ADD_PROTO_HANDLER_ENTRY(ppp_protos, PPP_IPV6CP, PROTOCOL_LAYER_NETWORK, dissect_icmpv6, "IPv6 Control Protocol");
}