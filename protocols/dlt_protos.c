#include "dlt_protos.h"

#include <stddef.h>
#include <pcap/dlt.h>

#include "../utils/hashmap.h"
#include "../libs/libnetdump/protocol.h"
#include "datalink/ether.h"
#include "datalink/ppp.h"
#include "datalink/frame_relay.h"
#include "network/ip.h"


hashmap *dlt_protos = NULL;

void load_dlt_protos(void) {
    dlt_protos = create_hashmap(DLT_PROTOS_BUCKETS_NUM);
    ADD_PROTO_HANDLER_ENTRY(dlt_protos, DLT_EN10MB, PROTOCOL_LAYER_DATALINK, dissect_ether, "Ethernet");
    ADD_PROTO_HANDLER_ENTRY(dlt_protos, DLT_PPP, PROTOCOL_LAYER_DATALINK, dissect_ppp, "PPP");
    ADD_PROTO_HANDLER_ENTRY(dlt_protos, DLT_IPV4, PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4");
    ADD_PROTO_HANDLER_ENTRY(dlt_protos, DLT_FRELAY, PROTOCOL_LAYER_DATALINK, dissect_frelay, "Frame Relay");
}