#include "nlpid_protos.h"

#include <stddef.h>

#include "../utils/hashmap.h"
#include "../utils/protocol.h"
#include "datalink/ppp.h"
#include "datalink/snap.h"
#include "network/ip.h"
#include "network/ipv6.h"


hashmap *nlpid_protos = NULL;

void load_nlpid_protos(void) {
    nlpid_protos = create_hashmap(NLPID_PROTOS_BUCKETS_NUM);
    ADD_PROTO_HANDLER_ENTRY(nlpid_protos, NLPID_SNAP, PROTOCOL_LAYER_DATALINK, dissect_snap, "SNAP");
    ADD_PROTO_HANDLER_ENTRY(nlpid_protos, NLPID_IP, PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4");
    ADD_PROTO_HANDLER_ENTRY(nlpid_protos, NLPID_PPP, PROTOCOL_LAYER_DATALINK, dissect_ppp, "PPP");
    ADD_PROTO_HANDLER_ENTRY(nlpid_protos, NLPID_IP6, PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6");
}