#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#define UNSCANNED_NET_HINT      "You are probably trying to print or visualize a packet without first scanning the network, run 'analize' command to scan the network"
#define UNCOMPATIBLE_TERMINAL_HINT "Check if your terminal supports ANSI escape sequences"

typedef enum {
    USER_NOT_ROOT_ERROR,
    UNKNOWN_COMMAND_ERROR,
    COMMAND_NOT_SUPPORTED_ERROR,
    WRONG_OPTIONS_FORMAT_ERROR,
    NEGATIVE_N_PACKETS,
    NULL_POINTER,
    INPUT_ERROR,
    INDEX_OUT_OF_BOUNDS,
    NO_DEVICE_FOUND,
    NETMASK_ERROR,
    NO_ACCESS_DEVICE_ERROR,
    DATALINK_HEADER_ERROR,
    INVALID_FILTER,
    NOT_INTALLABLE_FILTER,
    PCAP_LOOP_ERROR,
    TOO_MANY_ARGS,
    MISSING_ARGS_ERROR,
    UNRECOGNIZED_ARGS_ERROR,
    BUFFER_OVERFLOW_ERROR,
    NEGATIVE_BUFFER_INDEX,
    CURSOR_POSITION_ERROR,
    TERMINAL_SIZE_ERROR
} err_code;

typedef enum {
    ARRAY_RESET_SUCCESS,
    ARRAY_EMPTY_SUCCESS
} success_code;

char * get_error_format(err_code code);
void raise_error(err_code code, int should_exit, char *hint, ...);

char * get_success_msg(success_code code);
void print_success_msg(success_code code);

#endif