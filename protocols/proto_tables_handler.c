#include "proto_tables_handler.h"

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

static protocol_handler *proto_tables[PROTO_TABLE_COUNT] = {
    [DLT_PROTOS] = dlt_protos,
    [ETHERTYPES] = ethertypes,
    [IP_PROTOS] = ip_protos,
    [NET_PORTS] = net_ports,
    [NLPID_PROTOS] = nlpid_protos,
    [PPP_PROTOS] = ppp_protos
};

static const char *proto_table_labels[PROTO_TABLE_COUNT] = {
    [DLT_PROTOS] = DLT_PROTOS_LABEL,
    [ETHERTYPES] = ETHERTYPES_LABEL,
    [IP_PROTOS] = IP_PROTOS_LABEL,
    [NET_PORTS] = NET_PORTS_LABEL,
    [NLPID_PROTOS] = NLPID_PROTOS_LABEL,
    [PPP_PROTOS] = PPP_PROTOS_LABEL
};

proto_table_id _get_id_from_proto_table(protocol_handler *target_table);
proto_table_id _get_id_from_label(const char *label);


proto_table_id _get_id_from_proto_table(protocol_handler *target_table) {
    proto_table_id i;
    
    for (i = 0; i < PROTO_TABLE_COUNT; i ++) {
        if (proto_tables[i] == target_table) return i; 
    }
    return PROTO_TABLE_COUNT;  /* sentinel value, in this case means "table not found" */
}

proto_table_id _get_id_from_label(const char *target_label) {
    proto_table_id i;

    CHECK_NULL_RET(target_label, PROTO_TABLE_COUNT);
    for (i = 0; i < PROTO_TABLE_COUNT; i ++) {
        if (0 == strcmp(proto_table_labels[i], target_label)) return i; 
    }
    return PROTO_TABLE_COUNT;  /* sentinel value, in this case means "label not found" */
}


const char *get_table_label_from_id(proto_table_id id) {
    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) return NULL;
    return proto_table_labels[id];
}

const char *get_table_label_from_table(protocol_handler *target_table) {
    proto_table_id id = _get_id_from_proto_table(target_table);

    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) return UNKNOWN;
    return proto_table_labels[id];
}

protocol_handler *get_proto_table_from_id(proto_table_id id) {
    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) return NULL;
    return proto_tables[id];
}

protocol_handler *get_proto_table_from_name(const char *name) {
    char *lower_str;
    protocol_handler *res = NULL;
    proto_table_id id;

    CHECK_NULL_EXIT(name);
    lower_str = new_lower_str(name);

    id = _get_id_from_label(lower_str);
    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) res = NULL;
    else res = proto_tables[id];
    
    free(lower_str);
    return res;
}