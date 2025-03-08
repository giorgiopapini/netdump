#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#define UNKNOWN_COMMAND_HINT "Run 'help' command for assistance"
#define NO_DEVICE_HINT "The selected device may not exist, or there could be an issue with your internet connection."
#define UNSCANNED_NET_HINT "You are probably trying to print or visualize a packet without first scanning the network, run 'analize' command to scan the network"
#define UNCOMPATIBLE_TERMINAL_HINT "Check if your terminal supports ANSI escape sequences"
#define ABSOLUTE_PATH_HINT "Make sure to start with the '/' character if you intend to insert an absolute path"

typedef enum {
    USER_NOT_ROOT_ERROR,
    SELECT_FAILED_ERROR,
    UNKNOWN_COMMAND_ERROR,
    COMMAND_NOT_SUPPORTED_ERROR,
    WRONG_OPTIONS_FORMAT_ERROR,
    NEGATIVE_N_PACKETS,
    SAVING_EMPTY_PACKETS_ERROR,
    NULL_POINTER,
    INPUT_ERROR,
    INDEX_OUT_OF_BOUNDS,
    PCAP_FILE_ERROR,
    PCAP_HANDLE_ERROR,
    PCAP_DUMP_FILE_ERROR,
    NO_DEVICE_FOUND,
    DEVICES_SCAN_ERROR,
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
    TERMINAL_SIZE_ERROR,
    CURR_WORK_DIR_ERROR,
    CSTM_DISSECTORS_FOLDER_ERROR,
    LOADING_SHARED_LIB_ERROR,
    FUNCTION_NOT_FOUND_ERROR,
} err_code;

typedef enum {
    ARRAY_RESET_SUCCESS,
    ARRAY_EMPTY_SUCCESS,
    PACKETS_DUMP_SUCCESS
} success_code;

char * get_error_format(err_code code);
void raise_error(err_code code, int should_exit, char *hint, ...);

char * get_success_msg(success_code code);
void print_success_msg(success_code code);

#endif