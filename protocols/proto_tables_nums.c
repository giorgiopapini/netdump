#include "proto_tables_nums.h"

#include <stddef.h>

#include "../utils/formats.h"
#include "../utils/protocol.h"
#include "dlt_protos.h"
#include "ethertypes.h"
#include "ip_protos.h"
#include "net_ports.h"
#include "nlpid_protos.h"
#include "ppp_protos.h"


protocol_handler *get_proto_table(int proto_table_num) {
    switch (proto_table_num) {
        case DLT_PROTOS:        return dlt_protos;
        case ETHERTYPES:        return ethertypes;
        case IP_PROTOS:         return ip_protos;
        case NET_PORTS:         return net_ports;
        case NLPID_PROTOS:      return nlpid_protos;
        case PPP_PROTOS:        return ppp_protos;
        default:                return NULL;
    }
}

const char *get_table_name(protocol_handler *proto_table) {
    if (proto_table == dlt_protos)          return DLT_PROTOS_LABEL;
    else if (proto_table == ethertypes)     return ETHERTYPES_LABEL;
    else if (proto_table == ip_protos)      return IP_PROTOS_LABEL;
    else if (proto_table == net_ports)      return NET_PORTS_LABEL;
    else if (proto_table == nlpid_protos)   return NLPID_PROTOS_LABEL;
    else if (proto_table == ppp_protos)     return PPP_PROTOS_LABEL;
    
    return UNKNOWN;
}