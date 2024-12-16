#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "status_handler.h"
#include "utils/colors.h"


char * get_error_format(err_code code) {
    switch(code) {
        case USER_NOT_ROOT_ERROR:           return "App needs root privileges to execute packet scanning"; break;
        case UNKNOWN_COMMAND_ERROR:         return "No such command '%s'"; break; 
        case COMMAND_NOT_SUPPORTED_ERROR:   return "Your OS does not support the '%s' command"; break;
        case WRONG_OPTIONS_FORMAT_ERROR:    return "Invalid argument format. Usage: <command> -<option> <args>"; break;
        case NEGATIVE_N_PACKETS:            return "Packets number cannot be a negative quantity '(%d < 0)'"; break;
        case NULL_POINTER:                  return "NULL pointer when allocating '%s' in file '%s'"; break;
        case INPUT_ERROR:                   return "An error occured while reading input bytes"; break;
        case INDEX_OUT_OF_BOUNDS:           return "Tried to access packet number '%d' but array was '%d' packets long"; break;
        case PCAP_FILE_ERROR:               return "Couldn't open '%s' file"; break;
        case NO_DEVICE_FOUND:               return "Couldn't find default device"; break;
        case NETMASK_ERROR:                 return "Couldn't get netmask for device '%s'"; break;
        case NO_ACCESS_DEVICE_ERROR:        return "Couldn't open device '%s'"; break;
        case DATALINK_HEADER_ERROR:         return "Couldn't get datalink header type. (%s)"; break;
        case INVALID_FILTER:                return "Couldn't parse filter '%s'"; break;
        case NOT_INTALLABLE_FILTER:         return "Couldn't install filter '%s'"; break;
        case PCAP_LOOP_ERROR:               return "pcap_loop() failed"; break;
        case TOO_MANY_ARGS:                 return "Too many args (MAX_ARGS=%d)"; break;
        case MISSING_ARGS_ERROR:            return "Missing arguments: %s"; break;
        case UNRECOGNIZED_ARGS_ERROR:       return "Unrecognized arguments: %s"; break;
        case BUFFER_OVERFLOW_ERROR:         return "Buffer overflowed in file '%s' (max characters allowed = %d)"; break;
        case NEGATIVE_BUFFER_INDEX:         return "Buffer index cannot be a negative number '(%d < 0)'"; break;
        case CURSOR_POSITION_ERROR:         return "Couldn't retrieve the current cursor position"; break;
        case TERMINAL_SIZE_ERROR:           return "Couldn't retrieve the current terminal size"; break;
        default:                            return "Unkown error, please report this issue to mantainers"; break;
    }
}

char * get_success_msg(success_code code) {
    switch(code) {
        case ARRAY_RESET_SUCCESS:           return "Array resetted correctly."; break;
        case ARRAY_EMPTY_SUCCESS:           return "Array alredy empty"; break;
        default:                            return "Operation succeded"; break;
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
        printf(RESET YELLOW "(%s)", hint);
    }
    printf(".\n" RESET);

    va_end(args);

    if (should_exit) exit(EXIT_FAILURE);
    /* instead of writing trivial goto mechanism simply let the OS deallocate the remaining memory (this happens only in case
        of errors) */
}

void print_success_msg(success_code code) {
    const char *msg = get_success_msg(code);
    printf(GREEN "[SUCCESS] -> %s\n" RESET, msg);
}