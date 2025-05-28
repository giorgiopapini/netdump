#ifndef PROTO_TABLES_NUMS_H
#define PROTO_TABLES_NUMS_H

#include "../utils/protocol.h"

#define DLT_PROTOS          1
#define ETHERTYPES          2
#define IP_PROTOS           3
#define NET_PORTS           4
#define NLPID_PROTOS        5
#define PPP_PROTOS          6

/* if MACROS are not in lowercase, some functions like get_table_from_name will no longer work */
#define DLT_PROTOS_LABEL    "dlt_protos"
#define ETHERTYPES_LABEL    "ethertypes"
#define IP_PROTOS_LABEL     "ip_protos"
#define NET_PORTS_LABEL     "net_ports"
#define NLPID_PROTOS_LABEL  "nlpid_protos"
#define PPP_PROTOS_LABEL    "ppp_protos"


protocol_handler *get_proto_table(int proto_table_num);
const char *get_table_name(protocol_handler *proto_table);
protocol_handler *get_proto_table_from_name(const char *name);

#endif