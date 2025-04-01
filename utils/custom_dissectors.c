#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>

#include "custom_dissectors.h"
#include "../status_handler.h"
#include "../protocols/proto_tables_nums.h"
#include "../protocols/dlt_protos.h"
#include "../protocols/ethertypes.h"
#include "../protocols/ip_protos.h"
#include "../protocols/net_ports.h"
#include "../protocols/nlpid_protos.h"
#include "../protocols/ppp_protos.h"


custom_dissectors *create_custom_dissectors() {
    custom_dissectors *dissectors = (custom_dissectors *)malloc(sizeof(custom_dissectors));
    if (NULL == dissectors) raise_error(NULL_POINTER, 1, NULL, "dissectors", __FILE__);
    dissectors->len = 0;
    dissectors->table = NULL;
    return dissectors;
}

void add_custom_proto(dissectors_entry *arr, protocol_handler *new_custom_proto) {
    int i;

    if (NULL == arr->custom_protos) {
        arr->custom_protos = (protocol_handler **)malloc(sizeof(protocol_handler *));
        arr->custom_protos[0] = new_custom_proto;
        arr->len = 1;
    }
    else {
        for (i = 0; i < arr->len; i ++) {
            if (arr->custom_protos[i]->protocol == new_custom_proto->protocol) {
                raise_error(
                    CSTM_DISSECTORS_CONFLICT_ERROR, 
                    0, 
                    CSTM_DISSECTORS_CONFLICT_HINT, 
                    arr->custom_protos[i]->protocol_name,
                    new_custom_proto->protocol_name,
                    arr->custom_protos[i]->protocol,
                    get_table_name(arr->proto_table)
                );
                return;
            }
        }

        arr->len ++;
        arr->custom_protos = realloc(arr->custom_protos, arr->len * sizeof(protocol_handler *));
        arr->custom_protos[arr->len - 1] = new_custom_proto;
    }
}

dissectors_entry *create_dissectors_entry(protocol_handler *proto_table, protocol_handler *new_custom_proto, char *filename) {
    dissectors_entry *new_entry = (dissectors_entry *)malloc(sizeof(dissectors_entry));
    if (NULL == new_entry) raise_error(NULL_POINTER, 1, NULL, "new_entry", __FILE__);
    if (NULL == proto_table) raise_error(NULL_POINTER, 1, NULL, "proto_table", __FILE__);
    if (NULL == new_custom_proto) raise_error(NULL_POINTER, 1, NULL, "new_custom_proto", __FILE__);
    
    new_entry->proto_table = proto_table;
    new_entry->custom_protos = NULL;
    new_entry->lib_filename = filename;
    new_entry->len = 0;

    add_custom_proto(new_entry, new_custom_proto);
    return new_entry;
}

void add_dissector_entry(custom_dissectors *custom_dissectors, dissectors_entry *new_dissectors_entry) {
    int i;
    
    if (NULL == custom_dissectors->table) {
        custom_dissectors->table = (dissectors_entry **)malloc(sizeof(dissectors_entry *));
        custom_dissectors->table[0] = new_dissectors_entry;
        custom_dissectors->len = 1;
    }
    else {
        custom_dissectors->len ++;
        custom_dissectors->table = realloc(custom_dissectors->table, custom_dissectors->len * sizeof(dissectors_entry *));
        custom_dissectors->table[custom_dissectors->len - 1] = new_dissectors_entry;
    }
}

void dissector_add(protocol_handler *custom_handler, int dest_table_val, custom_dissectors *custom_dissectors, char *filename) {
    if (NULL == custom_dissectors) raise_error(NULL_POINTER, 1, NULL, "custom_dissectors", __FILE__);

    protocol_handler *dest_table = get_proto_table(dest_table_val);
    int i;

    if (NULL == custom_dissectors->table) add_dissector_entry(custom_dissectors, create_dissectors_entry(dest_table, custom_handler, filename));
    else {
        for (i = 0; i < custom_dissectors->len; i ++) {
            if (NULL != custom_dissectors->table[i] && custom_dissectors->table[i]->proto_table == dest_table) {
                add_custom_proto(custom_dissectors->table[i], custom_handler);
                return;
            }
        }
        add_dissector_entry(custom_dissectors, create_dissectors_entry(dest_table, custom_handler, filename));
    }
}

void populate_custom_dissectors(custom_dissectors *dissectors, protocol_handler_mapping **mappings, char *filename) {
    int i;

    for (i = 0; mappings[i] != NULL; i ++) {
        dissector_add(
            mappings[i]->handler, 
            mappings[i]->proto_table_num, 
            dissectors,
            filename
        );
    }
}

void load_dissector(custom_dissectors *custom_dissectors, void *handle, char *filename) {
    protocol_handler_mapping **mappings;
    protocol_handler_mapping **(*get_custom_protocols_mapping)();
    char *error;
    int i;

    if (NULL == handle) return;
    if (NULL == custom_dissectors) raise_error(NULL_POINTER, 1, NULL, "custom_dissectors", __FILE__);

    get_custom_protocols_mapping = dlsym(handle, FUNCTION_NAME);

    error = dlerror();
    if (error != NULL) {
        raise_error(FUNCTION_NOT_FOUND_ERROR, 0, NULL, error);
        dlclose(handle);
        return;
    }

    mappings = get_custom_protocols_mapping();
    populate_custom_dissectors(custom_dissectors, mappings, filename);
    
    destroy_mappings(mappings);
}

protocol_handler *get_custom_protocol_handler(
    custom_dissectors *custom_dissectors, 
    int target_proto, 
    protocol_handler *proto_table,
    shared_libs *libs
) {
    dissectors_entry *curr_entry = NULL;
    protocol_handler *curr_handler = NULL;
    int i, j;
    
    if (NULL == custom_dissectors) return NULL;
    if (NULL == custom_dissectors->table) return NULL;

    for (i = 0; i < custom_dissectors->len; i ++) {
        curr_entry = custom_dissectors->table[i];
        if (is_active(libs, curr_entry->lib_filename) && curr_entry->proto_table == proto_table) {
            for (j = 0; j < curr_entry->len; j ++) {
                curr_handler = curr_entry->custom_protos[j];
                if (curr_handler->protocol == target_proto) return curr_handler;
            }
        }
    }
    return NULL;
}

void destroy_dissectors_entry(dissectors_entry *entry) {
    int i;
    if (entry == NULL) return;
    
    if (entry->custom_protos != NULL) {
        for (i = 0; i < entry->len; i ++) {
            free(entry->custom_protos[i]);
            entry->custom_protos[i] = NULL;
        }
        free(entry->custom_protos);
        entry->custom_protos = NULL;
    }
    /* free(curr_entry->lib_filename); it is alredy deallocated in shared_libs destroy function? */
    free(entry);
}

void destroy_custom_dissectors(custom_dissectors *custom_dissectors) {
    int i, j;
    dissectors_entry *curr_entry = NULL;

    if (NULL == custom_dissectors) return;
    if (NULL == custom_dissectors->table) return;

    for (i = 0; i < custom_dissectors->len; i ++) {
        curr_entry = custom_dissectors->table[i];
        destroy_dissectors_entry(curr_entry);
    }

    free(custom_dissectors->table);
    custom_dissectors->table = NULL;
    custom_dissectors->len = 0;

    free(custom_dissectors);
}