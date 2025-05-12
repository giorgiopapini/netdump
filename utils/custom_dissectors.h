#ifndef CUSTOM_DISSECTORS_H
#define CUSTOM_DISSECTORS_H

#include <stddef.h>

#include "protocol.h"
#include "../utils/shared_lib.h"

#define FUNCTION_NAME               "get_custom_protocols_mapping"


typedef struct dissectors_entry {  /* (e.g. [ net_ports(addr)=[{custom_proto_1}, {custom_proto_2}, ...], ]) */
    protocol_handler *proto_table;
    protocol_handler **custom_protos;
    char *lib_filename; 
    size_t len;  /* custom_protos length */
} dissectors_entry;

typedef struct custom_dissectors {
    dissectors_entry **table;
    size_t len;
} custom_dissectors;

custom_dissectors *create_custom_dissectors(void);
void add_custom_proto(dissectors_entry *arr, protocol_handler *new_custom_proto);
dissectors_entry *create_dissectors_entry(protocol_handler *proto_table, protocol_handler *new_custom_proto, char *filename);
void add_dissector_entry(custom_dissectors *custom_diss, dissectors_entry *new_dissectors_entry);
void dissector_add(protocol_handler *custom_handler, int dest_table_val, custom_dissectors *custom_diss, char *filename);
void load_dissector(custom_dissectors *custom_diss, void *handle, char *filename);
protocol_handler *get_custom_protocol_handler(
    custom_dissectors *custom_diss, 
    int target_proto,
    protocol_handler *proto_table, 
    shared_libs *libs
);
void destroy_dissectors_entry(dissectors_entry *entry);
void destroy_custom_dissectors(custom_dissectors *custom_diss);

#endif