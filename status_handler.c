#include "status_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utils/colors.h"
#include "utils/formats.h"


static const char *error_formats[] = {
    [SELECT_FAILED_ERROR] = "'select()' failed in file '%s'. Reason: '%s'",
    [UNKNOWN_COMMAND_ERROR] = "No such command '%s'",
    [COMMAND_NOT_SUPPORTED_ERROR] = "Your OS does not support the '%s' command",
    [WRONG_OPTIONS_FORMAT_ERROR] = "Invalid argument format. Usage: " COMMAND_FORMAT,
    [NEGATIVE_N_PACKETS] = "Packets number cannot be a negative quantity '(%d < 0)'",
    [SAVING_EMPTY_PACKETS_ERROR] = "Couldn't save packets because packet array is empty",
    [NULL_POINTER] = "NULL pointer '%s' in file '%s'",
    [INPUT_ERROR] = "An error occured while reading input bytes",
    [INDEX_OUT_OF_BOUNDS] = "Tried to access packet number '%d' but array was '%d' packets long",
    [PCAP_FILE_ERROR] = "Couldn't open '%s' file",
    [PCAP_HANDLE_ERROR] = "Couldn't open pcap handle",
    [PCAP_DUMP_FILE_ERROR] = "Couldn't open dump file",
    [PCAP_FINDALLDEVS_ERROR] = "Error in pcap_findalldevs: %s",
    [NO_DEVICE_FOUND] = "Couldn't find device '%s'",
    [DEVICES_SCAN_ERROR] = "Couldn't find devices",
    [NETMASK_ERROR] = "Couldn't get netmask for device '%s'",
    [NO_ACCESS_DEVICE_ERROR] = "Couldn't open device '%s'",
    [DATALINK_HEADER_ERROR] = "Couldn't get datalink header type. (%s)",
    [INVALID_FILTER] = "Couldn't parse filter '%s'",
    [NOT_INTALLABLE_FILTER] = "Couldn't install filter '%s'",
    [PCAP_LOOP_ERROR] = "pcap_loop() failed",
    [TOO_MANY_ARGS] = "Too many args (MAX_ARGS=%d)",
    [MISSING_ARGS_ERROR] = "Missing arguments: %s",
    [UNRECOGNIZED_ARGS_ERROR] = "Unrecognized arguments: %s",
    [UNRECOGNIZED_OUTPUT_ERROR] = "Unrecognized '%s' output mode",
    [BUFFER_OVERFLOW_ERROR] = "Buffer overflowed in file '%s' (max characters allowed = %d)",
    [NEGATIVE_BUFFER_INDEX] = "Buffer index cannot be a negative number '(%d < 0)'",
    [CURSOR_POSITION_ERROR] = "Couldn't retrieve the current cursor position",
    [TERMINAL_SIZE_ERROR] = "Couldn't retrieve the current terminal size",
    [TCGETATTR_ERROR] = "Unable to retrieve terminal attributes",
    [TCSETATTR_ERROR] = "Unable to set terminal attributes",
    [CURR_WORK_DIR_ERROR] = "Couldn't resolve current working directory",
    [DIR_OPEN_ERROR] = "Couldn't open '%s' directory",
    [CSTM_DISSECTORS_CONFLICT_ERROR] = "Dissectors conflict between '%s' and '%s', both have same key: %d in '%s' table",
    [LOADING_SHARED_LIB_ERROR] = "Error loading '%s' lib (%s)",
    [FILE_NOT_SHARED_LIB_ERROR] = "File '%s' is not a shared library",
    [FUNCTION_NOT_FOUND_ERROR] = "Error finding function (%s)",
    [LIB_NOT_FOUND_ERROR] = "Library '%s' not found",
    [NO_SUCH_FILE_OR_DIR_ERROR] = "No such file or directory (%s)",
    [DELETE_FILE_ERROR] = "Couldn't delete '%s'",
    [LONG_TO_INT_CAST_ERROR] = "Failed to cast from long to int: value exceeds the range of int",
    [INT_TO_CHAR_CAST_ERROR] = "Failed to cast from int to char: value exceeds the range of char",
    [NON_CLOSING_STR_ERROR] = "A string was not properly closed, expected a closing '%s'",
    [PROTO_TABLE_NOT_FOUND_ERROR] = "Protocols table named '%s' was not found",
    [PROTOCOL_NOT_FOUND_ERROR] = "Protocol (%d) not found in '%s' proto table",
    [UNKNOWN_ERROR] = "Unknown error, please report this issue to mantainers",
};

static const char *warning_formats[] = {
    [DISSECTORS_EMPTY_WARNING] = "No custom dissectors found (.so)",
    [UNKNOWN_WARNING] = "Unknown warning, please report this issue to mantainers"
};

static const char *success_formats[] = {
    [ARRAY_RESET_SUCCESS] = "Array resetted correctly.",
    [ARRAY_EMPTY_SUCCESS] = "Array alredy empty",
    [PACKETS_DUMP_SUCCESS] = "Packets have successfully been written to file",
    [DISSECTOR_ACTIVATED_SUCCESS] = "Custom dissector successfully activated",
    [DISSECTOR_DEACTIVATED_SUCCESS] = "Custom dissector successfully deactivated",
    [DISSECTOR_LOADED_SUCCESS] = "Custom dissector successfully loaded",
    [DISSECTOR_DELETED_SUCCESS] = "Custom dissector successfully deleted",
    [PROTOCOL_FOUND_SUCCESS] = "Protocol found"
};


void raise_error(err_code code, int should_exit, const char *hint, ...) {
    va_list args;
    va_start(args, hint);

    fprintf(stderr, RED "[ERROR] (code: %d) -> ", code);
    vfprintf(stderr, (const char *)error_formats[code], args);
    
    if (NULL != hint) {
        fprintf(stderr, ".\n");
        fprintf(stderr, RESET_COLOR YELLOW "(%s)", hint);
    }
    fprintf(stderr, ".\n" RESET_COLOR);
    
    va_end(args);

    if (should_exit) {
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    /* 
        instead of writing trivial goto mechanism simply let the OS deallocate the remaining memory
        (this happens only in case of errors)
    */
}

void print_warning_msg(warning_code code, int newline) {
    const char *msg = warning_formats[code];
    printf(YELLOW "[WARNING] -> %s" RESET_COLOR, msg);
    if (0 != newline) printf("\n");
}

void print_success_msg(success_code code, int newline) {
    const char *msg = success_formats[code];
    printf(GREEN "[SUCCESS] -> %s" RESET_COLOR, msg);
    if (0 != newline) printf("\n");
}