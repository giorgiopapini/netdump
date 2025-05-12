#include "nlpid_protos.h"

#include <stddef.h>

#include "../utils/protocol.h"
#include "datalink/ppp.h"
#include "datalink/snap.h"
#include "network/ip.h"
#include "network/ipv6.h"

protocol_handler nlpid_protos[] = {
    { NLPID_Q933,           PROTOCOL_LAYER_NONE, NULL, "ANSI T1.617 Annex D or ITU-T Q.933 Annex A" },
    { NLPID_LMI,            PROTOCOL_LAYER_NONE, NULL, "LMI" },
    { NLPID_SNAP,           PROTOCOL_LAYER_DATALINK, dissect_snap, "SNAP" },
    { NLPID_CLNP,           PROTOCOL_LAYER_NONE, NULL, "CLNP" },
    { NLPID_ESIS,           PROTOCOL_LAYER_NONE, NULL, "ESIS" },
    { NLPID_ISIS,           PROTOCOL_LAYER_NONE, NULL, "ISIS" },
    { NLPID_CONS,           PROTOCOL_LAYER_NONE, NULL, "CONS" },
    { NLPID_IDRP,           PROTOCOL_LAYER_NONE, NULL, "IDRP" },
    { NLPID_MFR,            PROTOCOL_LAYER_NONE, NULL, "MFR" },
    { NLPID_SPB,            PROTOCOL_LAYER_NONE, NULL, "SPB" },
    { NLPID_IP,             PROTOCOL_LAYER_NETWORK, dissect_ip, "IPv4" },
    { NLPID_PPP,            PROTOCOL_LAYER_DATALINK, dissect_ppp, "PPP" },
    { NLPID_X25_ESIS,       PROTOCOL_LAYER_NONE, NULL, "X25_ESIS" },
    { NLPID_IP6,            PROTOCOL_LAYER_NETWORK, dissect_ipv6, "IPv6" },
    NULL_PROTO_HANDLER
};