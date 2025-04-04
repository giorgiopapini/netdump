#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "status_handler.h"
#include "utils/colors.h"
#include "utils/formats.h"

char * get_error_format(err_code code) {
    switch(code) {
        case USER_NOT_ROOT_ERROR:               return "root privileges are required to perform packet scanning"; break;
        case SELECT_FAILED_ERROR:               return "'select()' failed in file '%s'. Reason: '%s'"; break;
        case UNKNOWN_COMMAND_ERROR:             return "No such command '%s'"; break; 
        case COMMAND_NOT_SUPPORTED_ERROR:       return "Your OS does not support the '%s' command"; break;
        case WRONG_OPTIONS_FORMAT_ERROR:        return "Invalid argument format. Usage: " COMMAND_FORMAT; break;
        case NEGATIVE_N_PACKETS:                return "Packets number cannot be a negative quantity '(%d < 0)'"; break;
        case SAVING_EMPTY_PACKETS_ERROR:        return "Couldn't save packets because packet array is empty"; break;
        case NULL_POINTER:                      return "NULL pointer when allocating '%s' in file '%s'"; break;
        case INPUT_ERROR:                       return "An error occured while reading input bytes"; break;
        case INDEX_OUT_OF_BOUNDS:               return "Tried to access packet number '%d' but array was '%d' packets long"; break;
        case PCAP_FILE_ERROR:                   return "Couldn't open '%s' file"; break;
        case PCAP_HANDLE_ERROR:                 return "Couldn't open pcap handle"; break;
        case PCAP_DUMP_FILE_ERROR:              return "Couldn't open dump file"; break;
        case NO_DEVICE_FOUND:                   return "Couldn't find device '%s'"; break;
        case DEVICES_SCAN_ERROR:                return "Couldn't find devices"; break;
        case NETMASK_ERROR:                     return "Couldn't get netmask for device '%s'"; break;
        case NO_ACCESS_DEVICE_ERROR:            return "Couldn't open device '%s'"; break;
        case DATALINK_HEADER_ERROR:             return "Couldn't get datalink header type. (%s)"; break;
        case INVALID_FILTER:                    return "Couldn't parse filter '%s'"; break;
        case NOT_INTALLABLE_FILTER:             return "Couldn't install filter '%s'"; break;
        case PCAP_LOOP_ERROR:                   return "pcap_loop() failed"; break;
        case TOO_MANY_ARGS:                     return "Too many args (MAX_ARGS=%d)"; break;
        case MISSING_ARGS_ERROR:                return "Missing arguments: %s"; break;
        case UNRECOGNIZED_ARGS_ERROR:           return "Unrecognized arguments: %s"; break;
        case BUFFER_OVERFLOW_ERROR:             return "Buffer overflowed isn file '%s' (max characters allowed = %d)"; break;
        case NEGATIVE_BUFFER_INDEX:             return "Buffer index cannot be a negative number '(%d < 0)'"; break;
        case CURSOR_POSITION_ERROR:             return "Couldn't retrieve the current cursor position"; break;
        case TERMINAL_SIZE_ERROR:               return "Couldn't retrieve the current terminal size"; break;
        case CURR_WORK_DIR_ERROR:               return "Couldn't resolve current working directory"; break;
        case FOLDER_OPEN_ERROR:                 return "Couldn't open '%s' folder"; break;
        case CSTM_DISSECTORS_CONFLICT_ERROR:    return "Dissectors conflict between '%s' and '%s', both have same key: %d in '%s' table"; break;
        case LOADING_SHARED_LIB_ERROR:          return "Error loading '%s' lib (%s)"; break;
        case FUNCTION_NOT_FOUND_ERROR:          return "Error finding function (%s)"; break;
        case LIB_NOT_FOUND_ERROR:               return "Library '%s' not found"; break;
        case DELETE_FILE_ERROR:                 return "Couldn't delete '%s'"; break;
        case FILE_COPY_ERROR:                   return "Couldn't copy '%s' to '%s'"; break;
        case FILE_OVERWRITE_ERROR:              return "'%s' already exists in '%s' directory"; break;
        default:                                return "Unkown error, please report this issue to mantainers"; break;
    }
}

char * get_warning_msg(warinng_code code) {
    switch(code) {
        case DISSECTORS_EMPTY_WARNING:          return "No custom dissectors found (.so)"; break;
        default:                                return NULL; break;
    }
}

char * get_success_msg(success_code code) {
    switch(code) {
        case ARRAY_RESET_SUCCESS:               return "Array resetted correctly."; break;
        case ARRAY_EMPTY_SUCCESS:               return "Array alredy empty"; break;
        case PACKETS_DUMP_SUCCESS:              return "Packets have successfully been written to file"; break;
        case DISSECTOR_ACTIVATED_SUCCESS:       return "Custom dissector successfully activated"; break;
        case DISSECTOR_DEACTIVATED_SUCCESS:     return "Custom dissector successfully deactivated"; break;
        case DISSECTOR_LOADED_SUCCESS:          return "Custom dissector successfully loaded"; break;
        case DISSECTOR_DELETED_SUCCESS:         return "Custom dissector successfully deleted"; break;
        default:                                return "Operation succeded"; break;
    }
}

void raise_error(err_code code, int should_exit, char *hint, ...) {
    const char *format = get_error_format(code);

    va_list args;
    va_start(args, hint);

    printf(RED "[ERROR] (code: %d) -> ", code);
    vprintf(format, args);
    if (NULL != hint) {
        printf(".\n");
        printf(RESET_COLOR YELLOW "(%s)", hint);
    }
    printf(".\n" RESET_COLOR);

    va_end(args);

    if (should_exit) exit(EXIT_FAILURE);
    /* instead of writing trivial goto mechanism simply let the OS deallocate the remaining memory (this happens only in case
        of errors) */
}

void print_warning_msg(warinng_code code) {
    const char *msg = get_warning_msg(code);
    printf(YELLOW "[WARNING] -> %s\n" RESET_COLOR, msg);
}

void print_success_msg(success_code code) {
    const char *msg = get_success_msg(code);
    printf(GREEN "[SUCCESS] -> %s\n" RESET_COLOR, msg);
}