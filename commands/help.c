#include "help.h"

#include <stdio.h>
#include <string.h>

#include "../utils/colors.h"
#include "../utils/formats.h"
#include "../status_handler.h"


void print_cmd(const char *cmd, const char *desc) {
    printf("\n" PREFIX_STR);
    if (NULL != cmd) printf(GREEN "%s" RESET_COLOR, cmd);
    if (NULL != desc) printf(" (%s)", desc);
}

void print_arg(const char *arg_value, const char *desc, const char *prefix_space, const char *prefix_str, const char *example) {
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

void analize_help(void) {
    print_cmd(ANALIZE_COMMAND, ANALIZE_COMMAND_DESC);

    print_arg(NUMBER_ARG, PACKET_AMOUNT_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NUMBER_ARG_EG);
    print_arg(FILTER_ARG, FILTER_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, FILTER_ARG_EG);
    print_arg(DEVICE_ARG, DEVICE_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, DEVICE_ARG_EG);
    print_arg(DATALINK_HDR_ARG, DATALINK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NETWORK_HDR_ARG, NETWORK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(TRANSPORT_HDR_ARG, TRANSPORT_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(APPLICATION_HDR_ARG, APPLICATION_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NO_PROM_ARG, NO_PROM_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NO_TIMESTAMP_ARG, NO_TIMESTAMP_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NO_PROTOCOL_NAME_ARG, NO_PROTOCOL_NAME_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(PACKET_NUM_ARG, PACKET_NUM_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(READ_FILE_ARG, READ_FILE_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, READ_FILE_ARG_EG);
    print_arg(WRITE_FILE_ARG, WRITE_FILE_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, WRITE_FILE_ARG_EG);
}

void devlist_help(void) {
    print_cmd(DEVICES_LIST_COMMAND, DEVICES_LIST_COMMAND_DESC);
}

void dissectors_help(void) {
    print_cmd(DISSECTORS_COMMAND, DISSECTORS_COMMAND_DESC);
    print_arg(DISSECTOR_LIST_ARG, DISSECTOR_LIST_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(ADD_DISSECTOR_ARG, ADD_DISSECTOR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, ADD_DISSECTOR_ARG_EG);
    print_arg(ACTIVATE_LIB_ARG, ACTIVATE_LIB_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, ACTIVATE_LIB_ARG_EG);
    print_arg(DEACTIVATE_LIB_ARG, DEACTIVATE_LIB_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, DEACTIVATE_LIB_ARG_EG);
}

void reset_help(void) {
    print_cmd(RESET_COMMAND, RESET_COMMAND_DESC);
}

void print_help(void) {
    print_cmd(PRINT_COMMAND, PRINT_COMMAND_DESC);

    print_arg(NUMBER_ARG, NUMBER_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NUMBER_ARG_EG);
    print_arg(DATALINK_HDR_ARG, DATALINK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NETWORK_HDR_ARG, NETWORK_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(TRANSPORT_HDR_ARG, TRANSPORT_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(APPLICATION_HDR_ARG, APPLICATION_HDR_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NO_TIMESTAMP_ARG, NO_TIMESTAMP_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(NO_PROTOCOL_NAME_ARG, NO_PROTOCOL_NAME_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, NULL);
    print_arg(OUTPUT_FORMAT_ARG, OUTPUT_FORMAT_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, OUTPUT_FORMAT_ARG_EG);
    print_arg(PACKET_NUM_ARG, PACKET_NUM_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, NULL);
}

void clear_help(void) {
    print_cmd(CLEAR_COMMAND, CLEAR_COMMAND_DESC);
}

void exit_help(void) {
    print_cmd(EXIT_COMMAND, EXIT_COMMAND_DESC);
}

void save_help(void) {
    print_cmd(SAVE_COMMAND, SAVE_COMMAND_DESC);

    print_arg(DEST_FILE_ARG, DEST_FILE_ARG_DESC, DEFAULT_SPACE, DEFAULT_PIPE, DEST_FILE_ARG_EG);
    print_arg(NUMBER_ARG, NUMBER_ARG_DESC, DEFAULT_SPACE, DEFAULT_CORNER, NUMBER_ARG_EG);
}

void execute_help(void) {
    printf("Default command format: " COMMAND_FORMAT "\n");

    analize_help();
    print_help();
    dissectors_help();
    save_help();
    reset_help();
    devlist_help();
    clear_help();
    exit_help();
    printf("\n\n");
}