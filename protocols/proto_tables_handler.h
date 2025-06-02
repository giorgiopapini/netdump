#ifndef proto_tables_handler_H
#define proto_tables_handler_H

#include "../utils/protocol.h"
#include "../utils/hashmap.h"


void load_proto_hashmaps(void);
void destroy_proto_hashmaps(void);
const char *get_table_label_from_id(proto_table_id id);
const char *get_table_label_from_table(hashmap *proto_table);
hashmap *get_proto_table_from_id(proto_table_id id);
hashmap *get_proto_table_from_name(const char *name);

#endif