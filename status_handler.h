#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#define UNKNOWN_COMMAND_HINT "Run 'help' command for assistance"
#define NO_DEVICE_HINT "The selected device may not exist, or there could be an issue with your internet connection"
#define NO_ACCESS_DEVICE_HINT "You might not have the necessary permissions to capture packets on this device"
#define UNSCANNED_NET_HINT "You are probably trying to print or visualize a packet without first scanning the network, run 'analize' command to scan the network"
#define UNCOMPATIBLE_TERMINAL_HINT "Check if your terminal supports ANSI escape sequences"
#define ABSOLUTE_PATH_HINT "Make sure to start with the '/' character if you intend to insert an absolute path"
#define CSTM_DISSECTORS_CONFLICT_HINT "Resolve the conflicts to ensure the program operates correctly (deterministically)"

typedef enum {
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
    PCAP_FINDALLDEVS_ERROR,
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
    DIR_OPEN_ERROR,
    CSTM_DISSECTORS_CONFLICT_ERROR,
    LOADING_SHARED_LIB_ERROR,
    FUNCTION_NOT_FOUND_ERROR,
    LIB_NOT_FOUND_ERROR,
    NO_SUCH_FILE_OR_DIR_ERROR,
    DELETE_FILE_ERROR,
    LONG_TO_INT_CAST_ERROR,
    INT_TO_CHAR_CAST_ERROR,
    UNKNOWN_ERROR
} err_code;

typedef enum {
    DISSECTORS_EMPTY_WARNING,
    UNKNOWN_WARNING
} warning_code;

typedef enum {
    ARRAY_RESET_SUCCESS,
    ARRAY_EMPTY_SUCCESS,
    PACKETS_DUMP_SUCCESS,
    DISSECTOR_ACTIVATED_SUCCESS,
    DISSECTOR_DEACTIVATED_SUCCESS,
    DISSECTOR_LOADED_SUCCESS,
    DISSECTOR_DELETED_SUCCESS
} success_code;


void raise_error(err_code code, int should_exit, const char *hint, ...);
void print_warning_msg(warning_code code);
void print_success_msg(success_code code);

#endif