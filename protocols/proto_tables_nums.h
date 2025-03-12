#ifndef PROTO_TABLES_NUMS_H
#define PROTO_TABLES_NUMS_H

#include "../utils/protocol.h"

#define DLT_PROTOS          1
#define ETHERTYPES          2
#define IP_PROTOS           3
#define NET_PORTS           4
#define NLPID_PROTOS        5
#define PPP_PROTOS          6

#define DLT_PROTOS_LABEL    "DLT_PROTOS"
#define ETHERTYPES_LABEL    "ETHERTYPES"
#define IP_PROTOS_LABEL     "IP_PROTOS"
#define NET_PORTS_LABEL     "NET_PORTS"
#define NLPID_PROTOS_LABEL  "NLPID_PROTOS"
#define PPP_PROTOS_LABEL    "PPP_PROTOS"


protocol_handler *get_proto_table(int dest_proto_table);
const char *get_table_name(protocol_handler *proto_table);

#endif