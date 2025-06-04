#include "protocols.h"

#include <stdio.h>
#include <stdlib.h>                                                // for free
#include <string.h>

#include "../protocols/proto_tables_handler.h"
#include "../status_handler.h"
#include "../utils/colors.h"
#include "../utils/formats.h"
#include "../utils/protocol.h"
#include "../utils/string_utils.h"
#include "../command_handler.h"
#include "../utils/command.h"
#include "../utils/hashmap.h"


static void _print_table_names(void);
static void _print_layer(protocol_layer layer);
static void _print_protos(hashmap *table);
static void _print_table(const char *name, hashmap *table);
static void _print_selected_tables(char *tables);
static void _print_all_tables(void);
static void _show_search_result(const protocol_handler *result, const char *table_name);
static void _proto_search_wrapper(const char *tables, const int proto);


static void _print_table_names(void) {
    size_t i;

    printf("\n");
    for (i = 0; i < PROTO_TABLE_COUNT; i ++) {
        printf(PREFIX_STR);
        printf(GREEN "%s\n" RESET_COLOR, get_table_label_from_id((proto_table_id)i));
        /* i < PROTO_TABLE_COUNT in this loop, so it is safe to cast */
    }
    printf("\n");
}

static void _print_layer(protocol_layer layer) {
    printf(YELLOW);
    switch (layer) {
        case PROTOCOL_LAYER_DATALINK:
            printf("datalink");
            break;
        case PROTOCOL_LAYER_NETWORK:
            printf("network");
            break;
        case PROTOCOL_LAYER_TRANSPORT:
            printf("transport");
            break;
        case PROTOCOL_LAYER_APPLICATION:
            printf("application");
            break;
        default: break;
    }
    printf(RESET_COLOR);
}

static void _print_protos(hashmap *table) {
    size_t i;
    hashmap_entry *tmp;
    protocol_handler *proto_hdl;

    if (NULL == table) return;

    for (i = 0; i < table->buckets_count; i ++) {
        tmp = table->buckets[i];
        while (NULL != tmp) {
            proto_hdl = ((protocol_handler *)tmp->value);
            if (NULL != proto_hdl && NULL != proto_hdl->dissect_proto) {
                printf("\n" DEFAULT_SPACE BAR);

                printf(CYAN " %s" RESET_COLOR, proto_hdl->protocol_name ? proto_hdl->protocol_name : UNKNOWN);
                printf(" {num: " YELLOW "%d" RESET_COLOR ",", proto_hdl->protocol);
                printf(" layer: ");
                _print_layer(proto_hdl->layer);
                printf("}");
                
            }
            tmp = tmp->next;
        }
    }
}

static void _print_table(const char *name, hashmap *table) {
    if (NULL == table) {
        raise_error(PROTO_TABLE_NOT_FOUND_ERROR, 0, NULL, name);
        return;
    }

    printf(PREFIX_STR);
    if (NULL != name) printf(GREEN "%s" RESET_COLOR, name);
    _print_protos(table);
    printf("\n\n");
}

static void _print_selected_tables(char *tables) {
    char *token;
    char *trimmed;

    CHECK_NULL_EXIT(tables);

    printf("\n");
    token = strtok(tables, STRINGS_SEPARATOR);
    while (NULL != token) {
        trimmed = get_trimmed_str(token, strlen(token));
        if (NULL != trimmed) _print_table(trimmed, get_proto_table_from_name(trimmed)); 
        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
}

static void _print_all_tables(void) {
    size_t i;

    printf("\n");
    for (i = 0; i < PROTO_TABLE_COUNT; i ++) {
        _print_table(
            get_table_label_from_id((proto_table_id)i),
            get_proto_table_from_id((proto_table_id)i)
        );
        /* i < PROTO_TABLE_COUNT, so it is safe to cast */
    }
}

static protocol_handler *_simple_proto_search(const char *table_name, const int proto) {
    hashmap *table;
    hashmap_entry *entry;

    table = get_proto_table_from_name(table_name);
    if (NULL == table) {
        raise_error(PROTO_TABLE_NOT_FOUND_ERROR, 0, NULL, table_name);
        return NULL;
    }

    entry = get_entry(table, proto);
    if (NULL != entry && NULL != entry->value) return ((protocol_handler *)entry->value);
    return NULL;
}

static void _show_search_result(const protocol_handler *result, const char *table_name) {
    if (NULL == result) return;

    printf(CYAN "%s" RESET_COLOR, result->protocol_name ? result->protocol_name : UNKNOWN);
    printf(" {proto_table: " YELLOW "%s" RESET_COLOR, table_name);
    printf(", num: " YELLOW "%d" RESET_COLOR, result->protocol);
    printf(", layer: ");
    _print_layer(result->layer);
    printf("}\n");
}

static void _proto_search_wrapper(const char *tables, const int proto) {
    /* if tables == NULL than search in every table, otherwise search in selected tables */
    size_t i;
    char *token;
    char *trimmed = NULL;
    char *tables_copy = NULL;
    const char *table_label = NULL;
    protocol_handler *res = NULL;

    if (NULL == tables) {
        for (i = 0; i < PROTO_TABLE_COUNT; i ++) {
            /* i < PROTO_TABLE_COUNT, so it is safe to cast */
            table_label = get_table_label_from_id((proto_table_id)i);
            res = _simple_proto_search(table_label, proto);
            _show_search_result(res, table_label);
        }
    }
    else {
        tables_copy = strdup(tables);  /* otherwise strtok will modify the original tables string */
        CHECK_NULL_EXIT(tables_copy);

        token = strtok(tables_copy, STRINGS_SEPARATOR);
        while (NULL != token) {
            trimmed = get_trimmed_str(token, strlen(token));
            res = _simple_proto_search(trimmed, proto);

            if (NULL == res) raise_error(PROTOCOL_NOT_FOUND_ERROR, 0, NULL, proto, trimmed);
            else _show_search_result(res, trimmed);

            if (NULL != trimmed) free(trimmed);
            token = strtok(NULL, STRINGS_SEPARATOR);
        }

        free(tables_copy);
    }
}

void execute_protocols(command *cmd) {
    arg *tables_arg = get_arg(cmd, PROTO_TABLES_ARG);
    arg *target_proto_arg = get_arg(cmd, PROTO_SEARCH_ARG);
    char *selected_tables = get_raw_val(cmd, PROTO_FROM_ARG);

    if (NULL != tables_arg) _print_table_names();
    else {
        if (NULL == target_proto_arg) {
            if (NULL == selected_tables) _print_all_tables();
            else _print_selected_tables(selected_tables);
        }
        else _proto_search_wrapper(selected_tables, long_to_int(str_to_long(target_proto_arg->val)));
    }
}