#include "proto_tables_handler.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../status_handler.h"
#include "../utils/hashmap.h"
#include "../utils/formats.h"
#include "../utils/string_utils.h"
#include "../libs/libnetdump/protocol.h"
#include "dlt_protos.h"
#include "ethertypes.h"
#include "ip_protos.h"
#include "net_ports.h"
#include "nlpid_protos.h"
#include "ppp_protos.h"


static hashmap *proto_tables[PROTO_TABLE_COUNT];
static const char *proto_table_labels[PROTO_TABLE_COUNT];

void _load_proto_hashmaps_table(void);
void _load_proto_hashmaps_labels(void);
proto_table_id _get_id_from_proto_table(hashmap *target_table);
proto_table_id _get_id_from_label(const char *label);


void _load_proto_hashmaps_table(void) {
    proto_tables[DLT_PROTOS] = dlt_protos;
    proto_tables[ETHERTYPES] = ethertypes;
    proto_tables[IP_PROTOS] = ip_protos;
    proto_tables[NET_PORTS] = net_ports;
    proto_tables[NLPID_PROTOS] = nlpid_protos;
    proto_tables[PPP_PROTOS] = ppp_protos;
}

void _load_proto_hashmaps_labels(void) {
    proto_table_labels[DLT_PROTOS] = DLT_PROTOS_LABEL;
    proto_table_labels[ETHERTYPES] = ETHERTYPES_LABEL;
    proto_table_labels[IP_PROTOS] = IP_PROTOS_LABEL;
    proto_table_labels[NET_PORTS] = NET_PORTS_LABEL;
    proto_table_labels[NLPID_PROTOS] = NLPID_PROTOS_LABEL;
    proto_table_labels[PPP_PROTOS] = PPP_PROTOS_LABEL;
}

void load_proto_hashmaps(void) {
    load_dlt_protos();
    load_ethertypes();
    load_ip_protos();
    load_net_ports();
    load_nlpid_protos();
    load_ppp_protos();
    _load_proto_hashmaps_table();
    _load_proto_hashmaps_labels();
}

void destroy_proto_hashmaps(void) {
    destroy_hashmap(dlt_protos, free);
    destroy_hashmap(ethertypes, free);
    destroy_hashmap(ip_protos, free);
    destroy_hashmap(net_ports, free);
    destroy_hashmap(nlpid_protos, free);
    destroy_hashmap(ppp_protos, free);
}

proto_table_id _get_id_from_proto_table(hashmap *target_table) {
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

const char *get_table_label_from_table(hashmap *target_table) {
    proto_table_id id = _get_id_from_proto_table(target_table);

    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) return UNKNOWN;
    return proto_table_labels[id];
}

hashmap *get_proto_table_from_id(proto_table_id id) {
    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) return NULL;
    return proto_tables[id];
}

hashmap *get_proto_table_from_name(const char *name) {
    char *lower_str;
    hashmap *res = NULL;
    proto_table_id id;

    CHECK_NULL_EXIT(name);
    lower_str = new_lower_str(name);

    id = _get_id_from_label(lower_str);
    if (0 > id || PROTO_TABLE_COUNT <= id || NULL == proto_tables[id]) res = NULL;
    else res = proto_tables[id];
    
    free(lower_str);
    return res;
}