#include "proto_tables_nums.h"

#include "../utils/formats.h"
#include "../protocols/dlt_protos.h"
#include "../protocols/ethertypes.h"
#include "../protocols/ip_protos.h"
#include "../protocols/net_ports.h"
#include "../protocols/nlpid_protos.h"
#include "../protocols/ppp_protos.h"

protocol_handler *get_proto_table(int dest_proto_table) {
    switch (dest_proto_table) {
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