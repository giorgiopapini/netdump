#include "dlt_protos.h"

#include "protocols/datalink/ether.h"
#include "protocols/datalink/ppp.h"

protocol_handler dlt_protos[] = {
    { DLT_EN10MB,       PROTOCOL_LAYER_DATALINK, dissect_ether, "Ethernet" },
    { DLT_PPP,          PROTOCOL_LAYER_DATALINK,dissect_ppp, "PPP" },
    { 0,                PROTOCOL_LAYER_NONE, NULL, NULL },
};