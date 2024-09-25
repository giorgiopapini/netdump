#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/* Add the __FILE__ macro to every raise() call, by doing so the program knows where the error happened */

typedef enum {
    USER_NOT_ROOT_ERROR,
    UNKNOWN_COMMAND_ERROR,
    COMMAND_NOT_SUPPORTED_ERROR,
    WRONG_OPTIONS_FORMAT_ERROR,
    NEGATIVE_N_PACKETS,
    NULL_POINTER,
    INPUT_ERROR,
    INDEX_OUT_OF_BOUNDS
} err_code;

typedef enum {
    ARRAY_RESET_SUCCESS,
    ARRAY_EMPTY_SUCCESS
} success_code;

char * get_error_format(err_code code);
void raise(err_code code, int should_exit, ...);

char * get_success_msg(success_code code);
void print_success_msg(success_code code);

#endif