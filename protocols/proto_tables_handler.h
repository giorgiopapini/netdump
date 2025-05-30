#ifndef proto_tables_handler_H
#define proto_tables_handler_H

#include "../utils/protocol.h"

/* label should correspond to the actuals struct name */
#define DLT_PROTOS_LABEL    "dlt_protos"
#define ETHERTYPES_LABEL    "ethertypes"
#define IP_PROTOS_LABEL     "ip_protos"
#define NET_PORTS_LABEL     "net_ports"
#define NLPID_PROTOS_LABEL  "nlpid_protos"
#define PPP_PROTOS_LABEL    "ppp_protos"

typedef enum {
    DLT_PROTOS,
    ETHERTYPES,
    IP_PROTOS,
    NET_PORTS,
    NLPID_PROTOS,
    PPP_PROTOS,
    PROTO_TABLE_COUNT  /* sentinel value */
} proto_table_id;


const char *get_table_label_from_id(proto_table_id id);
const char *get_table_label_from_table(protocol_handler *proto_table);
protocol_handler *get_proto_table_from_id(proto_table_id id);
protocol_handler *get_proto_table_from_name(const char *name);

#endif