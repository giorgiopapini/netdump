#include "dlt_protos.h"

#include <pcap/dlt.h>

#include "../utils/protocol.h"
#include "datalink/ether.h"
#include "datalink/ppp.h"
#include "datalink/frame_relay.h"
#include "network/ip.h"

protocol_handler dlt_protos[] = {
    { DLT_EN10MB,       PROTOCOL_LAYER_DATALINK, dissect_ether, "Ethernet" },
    { DLT_PPP,          PROTOCOL_LAYER_DATALINK, dissect_ppp, "PPP" },
    { DLT_IPV4,         PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4" },
    { DLT_FRELAY,       PROTOCOL_LAYER_DATALINK, dissect_frelay, "Frame Relay" },
    NULL_PROTO_HANDLER
};