#include "help.h"

#include <stdio.h>
#include <string.h>

#include "../command_handler.h"
#include "../status_handler.h"
#include "../utils/formats.h"
#include "../utils/colors.h"

static void _print_cmd(const char *cmd, const char *desc);
static void _print_arg(
    const char *arg_value,
    const char *desc,
    const char *prefix_space,
    const char *prefix_str,
    const char *example
);

static void _analize_help(void);
static void _devlist_help(void);
static void _protolist_help(void);
static void _dissectors_help(void);
static void _reset_help(void);
static void _print_help(void);
static void _clear_help(void);
static void _exit_help(void);
static void _save_help(void);


static void _print_cmd(const char *cmd, const char *desc) {
    printf("\n" PREFIX_STR);
    if (NULL != cmd) printf(GREEN "%s" RESET_COLOR, cmd);
    if (NULL != desc) printf(" (%s)", desc);
}

static void _print_arg(
    const char *arg_value,
    const char *desc,
    const char *prefix_space,
    const char *prefix_str,
    const char *example
) {
    printf("\n");
    if (NULL != prefix_space) printf("%s", prefix_space);
    if (NULL != prefix_str) printf("%s", prefix_str);
    if (NULL != arg_value) printf(CYAN " %s" RESET_COLOR, arg_value);
    if (NULL != desc) printf(" (%s)", desc);
    if (NULL != example) printf(YELLOW " (e.g. '%s')" RESET_COLOR, example);

    CHECK_NULL_RET(prefix_str);
    CHECK_NULL_RET(DEFAULT_CORNER);
    if (0 == strcmp(prefix_str, DEFAULT_CORNER)) printf("\n");
}

static void _analize_help(void) {
    _print_cmd(ANALYZE_COMMAND, ANALYZE_COMMAND_DESC);

    _print_arg(NUMBER_ARG, PACKET_AMOUNT_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NUMBER_ARG_EG);
    _print_arg(FILTER_ARG, FILTER_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, FILTER_ARG_EG);
    _print_arg(DEVICE_ARG, DEVICE_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, DEVICE_ARG_EG);
    _print_arg(DATALINK_HDR_ARG, DATALINK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NETWORK_HDR_ARG, NETWORK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(TRANSPORT_HDR_ARG, TRANSPORT_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(APPLICATION_HDR_ARG, APPLICATION_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NO_PROM_ARG, NO_PROM_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NO_TIMESTAMP_ARG, NO_TIMESTAMP_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NO_PROTOCOL_NAME_ARG, NO_PROTOCOL_NAME_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(PACKET_NUM_ARG, PACKET_NUM_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(READ_FILE_ARG, READ_FILE_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, READ_FILE_ARG_EG);
    _print_arg(WRITE_FILE_ARG, WRITE_FILE_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, WRITE_FILE_ARG_EG);
}

static void _devlist_help(void) {
    _print_cmd(DEVICES_LIST_COMMAND, DEVICES_LIST_COMMAND_DESC);
}

static void _protolist_help(void) {
    _print_cmd(PROTOCOLS_COMMAND, PROTOCOLS_COMMAND_DESC);

    _print_arg(PROTO_TABLES_ARG, PROTO_TABLES_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(PROTO_FROM_ARG, PROTO_FROM_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, PROTO_FROM_ARG_EG);
    _print_arg(PROTO_SEARCH_ARG, PROTO_SEARCH_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, NULL);
}

static void _dissectors_help(void) {
    _print_cmd(DISSECTORS_COMMAND, DISSECTORS_COMMAND_DESC);

    _print_arg(DISSECTOR_LIST_ARG, DISSECTOR_LIST_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(ADD_DISSECTOR_ARG, ADD_DISSECTOR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, ADD_DISSECTOR_ARG_EG);
    _print_arg(ACTIVATE_LIB_ARG, ACTIVATE_LIB_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, ACTIVATE_LIB_ARG_EG);
    _print_arg(DEACTIVATE_LIB_ARG, DEACTIVATE_LIB_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, DEACTIVATE_LIB_ARG_EG);
}

static void _reset_help(void) {
    _print_cmd(RESET_COMMAND, RESET_COMMAND_DESC);
}

static void _print_help(void) {
    _print_cmd(PRINT_COMMAND, PRINT_COMMAND_DESC);

    _print_arg(NUMBER_ARG, NUMBER_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NUMBER_ARG_EG);
    _print_arg(DATALINK_HDR_ARG, DATALINK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NETWORK_HDR_ARG, NETWORK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(TRANSPORT_HDR_ARG, TRANSPORT_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(APPLICATION_HDR_ARG, APPLICATION_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NO_TIMESTAMP_ARG, NO_TIMESTAMP_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(NO_PROTOCOL_NAME_ARG, NO_PROTOCOL_NAME_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    _print_arg(OUTPUT_FORMAT_ARG, OUTPUT_FORMAT_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, OUTPUT_FORMAT_ARG_EG);
    _print_arg(PACKET_NUM_ARG, PACKET_NUM_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, NULL);
}

static void _clear_help(void) {
    _print_cmd(CLEAR_COMMAND, CLEAR_COMMAND_DESC);
}

static void _exit_help(void) {
    _print_cmd(EXIT_COMMAND, EXIT_COMMAND_DESC);
}

static void _save_help(void) {
    _print_cmd(SAVE_COMMAND, SAVE_COMMAND_DESC);

    _print_arg(DEST_FILE_ARG, DEST_FILE_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, DEST_FILE_ARG_EG);
    _print_arg(NUMBER_ARG, NUMBER_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, NUMBER_ARG_EG);
}

void execute_help(void) {
    printf("Default command format: " COMMAND_FORMAT "\n");

    _analize_help();
    _print_help();
    _dissectors_help();
    _save_help();
    _protolist_help();
    _devlist_help();
    _reset_help();
    _clear_help();
    _exit_help();
    printf("\n\n");
}