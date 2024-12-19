#include <stdio.h>
#include <string.h>

#include "help.h"
#include "../utils/colors.h"
#include "../command_handler.h"
#include "../utils/formats.h"


void print_cmd(char *cmd, char *desc) {
    printf("\n" PREFIX_STR);
    if (NULL != cmd) printf(GREEN "%s" RESET_COLOR, cmd);
    if (NULL != desc) printf(" (%s)", desc);
}

void print_arg(char *arg, char *desc, char *prefix_space, char *prefix_str) {
    printf("\n");
    if (NULL != prefix_space) printf("%s", prefix_space);
    if (NULL != prefix_str) printf("%s", prefix_str);
    if (NULL != arg) printf(" %s", arg);
    if (NULL != desc) printf(" (%s)", desc);
}

void analize_help() {
    print_cmd(ANALIZE_COMMAND, ANALIZE_COMMAND_DESC);

    print_arg(NUMBER_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(FILTER_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(DEVICE_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(DATALINK_HDR_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NETWORK_HDR_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NO_PROM_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NO_TIMESTAMP_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(PACKET_NUM_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(READ_FILE_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(WRITE_FILE_ARG, NULL, DEFAULT_SPACE, DEFAULT_CORNER);
}

void devlist_help() {
    print_cmd(DEVICES_LIST_COMMAND, DEVICES_LIST_COMMAND_DESC);
}

void reset_help() {
    print_cmd(RESET_COMMAND, RESET_COMMAND_DESC);
}

void print_help() {
    print_cmd(PRINT_COMMAND, PRINT_COMMAND_DESC);

    print_arg(NUMBER_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(DATALINK_HDR_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NETWORK_HDR_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NO_TIMESTAMP_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(PACKET_NUM_ARG, NULL, DEFAULT_SPACE, DEFAULT_CORNER);
}

void visualize_help() {
    print_cmd(VISUALIZE_COMMAND, VISUALIZE_COMMAND_DESC);

    print_arg(NUMBER_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(DATALINK_HDR_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NETWORK_HDR_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(NO_TIMESTAMP_ARG, NULL, DEFAULT_SPACE, DEFAULT_PIPE);
    print_arg(PACKET_NUM_ARG, NULL, DEFAULT_SPACE, DEFAULT_CORNER);
}

void clear_help() {
    print_cmd(CLEAR_COMMAND, CLEAR_COMMAND_DESC);
}

void exit_help() {
    print_cmd(EXIT_COMMAND, EXIT_COMMAND_DESC);
}

void execute_help(command *cmd) {
    printf("Default command format: " COMMAND_FORMAT "\n");

    analize_help();
    print_help();
    visualize_help();
    reset_help();
    devlist_help();
    clear_help();
    exit_help();
    printf("\n\n");
}