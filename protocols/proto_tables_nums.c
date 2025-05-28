#include "proto_tables_nums.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../status_handler.h"
#include "../utils/formats.h"
#include "../utils/protocol.h"
#include "../utils/string_utils.h"
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

protocol_handler *get_proto_table_from_name(const char *name) {
    char *lower_str;
    protocol_handler *res = NULL;

    CHECK_NULL_EXIT(name);
    lower_str = new_lower_str(name);

    /* this works until MACROS are lowercase */
    if (0 == strcmp(lower_str, DLT_PROTOS_LABEL))           res = dlt_protos;
    else if (0 == strcmp(lower_str, ETHERTYPES_LABEL))      res = ethertypes;
    else if (0 == strcmp(lower_str, IP_PROTOS_LABEL))       res = ip_protos;
    else if (0 == strcmp(lower_str, NET_PORTS_LABEL))       res = net_ports;
    else if (0 == strcmp(lower_str, NLPID_PROTOS_LABEL))    res = nlpid_protos;
    else if (0 == strcmp(lower_str, PPP_PROTOS_LABEL))      res = ppp_protos;
    
    free(lower_str);
    return res;
}