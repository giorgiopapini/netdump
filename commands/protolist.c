#include "protolist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../status_handler.h"
#include "../utils/protocol.h"
#include "../utils/formats.h"
#include "../utils/colors.h"
#include "../utils/string_utils.h"
#include "../protocols/dlt_protos.h"
#include "../protocols/ethertypes.h"
#include "../protocols/ppp_protos.h"
#include "../protocols/nlpid_protos.h"
#include "../protocols/ip_protos.h"
#include "../protocols/net_ports.h"
#include "../protocols/proto_tables_nums.h"

static void _print_layer(protocol_layer layer);
static void _print_protos(protocol_handler *table);
static void _print_table(const char *name, protocol_handler *table);
static void _print_selected_tables(char *tables);
static void _print_all_tables(void);


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

static void _print_protos(protocol_handler *table) {
    size_t i;
    size_t j = 0;

    if (NULL == table) return;

    for (i = 0; !IS_NULL_HANDLER(table[i]); i ++) {
        if (NULL != table[i].dissect_proto) {
            printf("\n");
            printf(DEFAULT_SPACE);

            /* discovers if table[i] is the last defined dissector */
            j = i + 1;
            while (NULL == table[j].dissect_proto && !IS_NULL_HANDLER(table[j])) j ++;

            if (IS_NULL_HANDLER(table[j])) printf(DEFAULT_CORNER);
            else printf(DEFAULT_PIPE);

            printf(CYAN " %s" RESET_COLOR, table[i].protocol_name ? table[i].protocol_name : UNKNOWN);
            printf(" (num: " YELLOW "%d" RESET_COLOR ")", table[i].protocol);
            printf(" (layer: ");
            _print_layer(table[i].layer);
            printf(")");
        }
    }
}

static void _print_table(const char *name, protocol_handler *table) {
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

    token = strtok(tables, STRINGS_SEPARATOR);
    while (NULL != token) {
        trimmed = get_trimmed_str(token, strlen(token));
        if (NULL != trimmed) _print_table(trimmed, get_proto_table_from_name(trimmed)); 
        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
}

static void _print_all_tables(void) {
    _print_table(DLT_PROTOS_LABEL, dlt_protos);
    _print_table(ETHERTYPES_LABEL, ethertypes);
    _print_table(PPP_PROTOS_LABEL, ppp_protos);
    _print_table(NLPID_PROTOS_LABEL, nlpid_protos);
    _print_table(IP_PROTOS_LABEL, ip_protos);
    _print_table(NET_PORTS_LABEL, net_ports);
}

void execute_protolist(command *cmd) {
    char *tables = get_raw_val(cmd, PROTO_TABLE_ARG);

    printf("\n");
    if (NULL != tables) _print_selected_tables(tables);
    else _print_all_tables();
}