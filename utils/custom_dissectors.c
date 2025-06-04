#include "custom_dissectors.h"

#include <dlfcn.h>
#include <stdlib.h>

#include "../protocols/proto_tables_handler.h"
#include "../status_handler.h"
#include "hashmap.h"
#include "shared_lib.h"


static int _populate_custom_dissectors(custom_dissectors *dissectors, protocol_handler_mapping **mappings, char *filename);

custom_dissectors *create_custom_dissectors(void) {
    custom_dissectors *dissectors = malloc(sizeof *dissectors);
    CHECK_NULL_EXIT(dissectors);

    dissectors->len = 0;
    dissectors->table = NULL;
    return dissectors;
}

void add_custom_proto(dissectors_entry *arr, protocol_handler *new_custom_proto) {
    size_t i;
    
    CHECK_NULL_EXIT(arr);
    if (NULL == arr->custom_protos) {
        arr->custom_protos = malloc(sizeof *(arr->custom_protos));
        CHECK_NULL_EXIT(arr->custom_protos);
        arr->custom_protos[0] = new_custom_proto;
        arr->len = 1;
    }
    else {
        for (i = 0; i < arr->len; i ++) {
            CHECK_NULL_RET(arr->custom_protos[i]);
            CHECK_NULL_RET(new_custom_proto);

            if (arr->custom_protos[i]->protocol == new_custom_proto->protocol) {
                raise_error(
                    CSTM_DISSECTORS_CONFLICT_ERROR, 
                    0, 
                    CSTM_DISSECTORS_CONFLICT_HINT, 
                    arr->custom_protos[i]->protocol_name,
                    new_custom_proto->protocol_name,
                    arr->custom_protos[i]->protocol,
                    get_table_label_from_table(arr->proto_table)
                );
                return;
            }
        }

        arr->len ++;
        arr->custom_protos = realloc(arr->custom_protos, arr->len * sizeof arr->custom_protos);
        CHECK_NULL_EXIT(arr->custom_protos);
        arr->custom_protos[arr->len - 1] = new_custom_proto;
    }
}

dissectors_entry *create_dissectors_entry(hashmap *proto_table, protocol_handler *new_custom_proto, char *filename) {
    dissectors_entry *new_entry = malloc(sizeof *new_entry);
    CHECK_NULL_EXIT(new_entry);
    CHECK_NULL_EXIT(proto_table);
    CHECK_NULL_EXIT(new_custom_proto);
    
    new_entry->proto_table = proto_table;
    new_entry->custom_protos = NULL;
    new_entry->lib_filename = filename;
    new_entry->len = 0;

    add_custom_proto(new_entry, new_custom_proto);
    return new_entry;
}

void add_dissector_entry(custom_dissectors *custom_diss, dissectors_entry *new_dissectors_entry) {
    CHECK_NULL_EXIT(custom_diss);
    if (NULL == custom_diss->table) {
        custom_diss->table = malloc(sizeof *(custom_diss->table));
        CHECK_NULL_EXIT(custom_diss->table);
        custom_diss->table[0] = new_dissectors_entry;
        custom_diss->len = 1;
    }
    else {
        custom_diss->len ++;
        custom_diss->table = realloc(custom_diss->table, custom_diss->len * sizeof custom_diss->table);
        CHECK_NULL_EXIT(custom_diss->table);
        custom_diss->table[custom_diss->len - 1] = new_dissectors_entry;
    }
}

int dissector_add(protocol_handler *custom_handler, int dest_table_val, custom_dissectors *custom_diss, char *filename) {
    hashmap *dest_table;
    size_t i;

    if (0 > dest_table_val || PROTO_TABLE_COUNT < dest_table_val) {
        raise_error(PROTO_TABLE_ID_NOT_FOUND_ERROR, 0, NULL, dest_table_val, 0, PROTO_TABLE_COUNT);
        free(custom_handler);  /* instead of loading the custom_handler, free it and return the error status */
        return 0;
    }

    /* safe casting, if dest_table_val not in PROTO_TABLE_COUNT range the error above gets triggered */
    dest_table = get_proto_table_from_id((proto_table_id)dest_table_val);
    CHECK_NULL_EXIT(custom_diss);

    if (NULL == custom_diss->table) add_dissector_entry(custom_diss, create_dissectors_entry(dest_table, custom_handler, filename));
    else {
        for (i = 0; i < custom_diss->len; i ++) {
            if (NULL != custom_diss->table[i] && custom_diss->table[i]->proto_table == dest_table) {
                add_custom_proto(custom_diss->table[i], custom_handler);
                return 1;
            }
        }
        add_dissector_entry(custom_diss, create_dissectors_entry(dest_table, custom_handler, filename));
    }
    return 1;
}

static int _populate_custom_dissectors(custom_dissectors *dissectors, protocol_handler_mapping **mappings, char *filename) {
    int status = 1;
    size_t i;

    CHECK_NULL_EXIT(mappings);
    for (i = 0; mappings[i] != NULL; i ++) {
        status = dissector_add(
            mappings[i]->handler, 
            mappings[i]->proto_table_num, 
            dissectors,
            filename
        );
        if (0 == status) return status;
    }
    return status;
}

int load_dissector(custom_dissectors *custom_diss, void *handle, char *filename) {
    protocol_handler_mapping **mappings;
    protocol_handler_mapping **(*get_custom_protocols_mapping)(void);
    char *error;
    int status = 1;

    CHECK_NULL_EXIT(custom_diss);
    CHECK_NULL_EXIT(handle);
    CHECK_NULL_EXIT(filename);

    get_custom_protocols_mapping = (protocol_handler_mapping **(*)(void))dlsym(handle, FUNCTION_NAME);

    error = dlerror();
    if (NULL != error) {
        raise_error(FUNCTION_NOT_FOUND_ERROR, 0, NULL, error);
        dlclose(handle);
        return 0;
    }

    mappings = get_custom_protocols_mapping();
    status = _populate_custom_dissectors(custom_diss, mappings, filename);

    destroy_mappings(mappings);
    return status;
}

protocol_handler *get_custom_protocol_handler(
    custom_dissectors *custom_diss, 
    int target_proto, 
    hashmap *proto_table,
    shared_libs *libs
) {
    dissectors_entry *curr_entry = NULL;
    protocol_handler *curr_handler = NULL;
    size_t i, j;
    
    if (NULL == custom_diss) return NULL;
    if (NULL == custom_diss->table) return NULL;

    for (i = 0; i < custom_diss->len; i ++) {
        curr_entry = custom_diss->table[i];
        if (NULL == curr_entry) continue;

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
    size_t i;
    if (NULL == entry) return;
    
    if (NULL != entry->custom_protos) {
        for (i = 0; i < entry->len; i ++) {
            free(entry->custom_protos[i]);
            entry->custom_protos[i] = NULL;
        }
        free(entry->custom_protos);
        entry->custom_protos = NULL;
    }
    if (NULL != entry->lib_filename) free(entry->lib_filename);
    free(entry);
}

void destroy_custom_dissectors(custom_dissectors *custom_diss) {
    size_t i;
    dissectors_entry *curr_entry = NULL;

    if (NULL == custom_diss) return;
    if (NULL == custom_diss->table) return;

    for (i = 0; i < custom_diss->len; i ++) {
        curr_entry = custom_diss->table[i];
        destroy_dissectors_entry(curr_entry);
    }

    free(custom_diss->table);
    custom_diss->table = NULL;
    custom_diss->len = 0;
}