#include "command_handler.h"

#include <stdlib.h>
#include <string.h>

#include "status_handler.h"
#include "utils/string_utils.h"
#include "utils/formats.h"
#include "command_handler.h"
#include "commands/analize.h"
#include "commands/devlist.h"
#include "commands/dissectors.h"
#include "commands/reset.h"
#include "commands/print.h"
#include "commands/clear.h"
#include "commands/save.h"
#include "commands/help.h"

#define CHECK_REQ_ARGS(cmd, ...)            (is_valid(cmd, 0, (const char *[]){__VA_ARGS__}, LEN(char *, __VA_ARGS__)))
#define CHECK_ARGS(cmd, ...)                (is_valid(cmd, 1, (const char *[]){__VA_ARGS__}, LEN(char *, __VA_ARGS__)))


int check_compliance(buffer *buff) {
    size_t i;

    CHECK_NULL_RET(buff, 1);
    CHECK_NULL_RET(buff->content, 1);

    for (i = 1; i < buff->len; i ++) {
        /* if char at (i - 1) is not empty and is not the leading char (which may happen, because of the buffer trim func) */
        if (buff->content[i - 1] == ' ' && 0 != (i - 1)) {
            if (buff->content[i] == ARG_PREFIX[0]) return 1;
            else return 0;
        }
    }

    return 1;
}

int create_cmd_from_buff(command *cmd, buffer *buff) {
    size_t args_num = 0;
    char *temp;
    size_t i;
    int j;
    int str_arg_value = 0;  /* flag to check if " already showed up */
    int writing_arg = 0;  /* flag to check if ARG_PREFIX alredy showed up */
    int status = 0;

    CHECK_NULL_RET(cmd, 1);
    CHECK_NULL_RET(buff, 1);

    temp = (char *)malloc(buff->len + 1);  /* including null terminator */
    CHECK_NULL_EXIT(temp);

    /* if there is <command> <arg> without the PREFIX separator than raise a formatting error */
    if (!check_compliance(buff)) {
        raise_error(WRONG_OPTIONS_FORMAT_ERROR, 0, NULL);
        return 1;
    }

    cmd->label = NULL;
    for (i = 0, j = 0; i < buff->len; i ++, j ++) {
        temp[j] = buff->content[i];
        if (NULL == cmd->label) {
            if (j > 0 && ' ' == temp[j]) {
                temp[j] = '\0';
                cmd->label = (char *)malloc((size_t)j + 1); /* (j > 0) is certain at this point */
                CHECK_NULL_EXIT(cmd->label);
                strcpy(cmd->label, temp);
                j = -1;
            }
        }
        else {
            if (STRING_DELIMITER == temp[j]) {
                temp[j] = ' ';
                j --;
                str_arg_value = !str_arg_value;
            }
            else if (ARG_PREFIX[0] == temp[j] && 0 == str_arg_value) {
                temp[j] = '\0';
                if (strlen(temp) > 0) {
                    status = add_arg_from_token(cmd, temp, &args_num);
                    if (status) return 1;
                    writing_arg = !writing_arg;
                }
                j = -1;  /* next iteration j=>0 */
            }
        }
    }

    if (0 > j) {
        raise_error(NEGATIVE_BUFFER_INDEX, 0, NULL, j);
        return 0;
    }

    temp[j] = '\0';
    if (NULL == cmd->label) {
        cmd->label = (char *)malloc((size_t)j + 1);
        CHECK_NULL_EXIT(cmd->label);
        strncpy(cmd->label, temp, (size_t)j);
        cmd->label[(size_t)j] = '\0';
    }
    else {
        if (strlen(temp) > 0) status = add_arg_from_token(cmd, temp, &args_num);
        if (NULL != temp) {
            free(temp);
            temp = NULL;
        }
        if (status) return 1;
    }

    if (NULL != temp) free(temp);
    return 0;
}

int is_valid(command *cmd, int opt_args, const char **expected_args, size_t len) {
    arg *tmp = NULL;
    int valid = 1;
    int not_in_arr;
    size_t i;
    size_t j = 0;  /* will represent length of missing_args array */
    size_t k;  
    size_t m = 0;  /* will represent length of unrecognized_args array */

    const char *missing_args[MAX_ARGS]; /* size_t len might represents the 'worst case length'?. Every arg misses so missing_args has length = len */
    char *missing_args_message = NULL;
    const char *unrecognized_args[MAX_ARGS];
    char *unrecognized_args_message = NULL;

    if (0 < len && 0 == strcmp(expected_args[0], NONE_ARG)) return valid;

    /* populate missing_args */
    for (i = 0; i < len; i ++) {
        tmp = get_arg(cmd, expected_args[i]);

        if (NULL == tmp) {
            missing_args[j] = expected_args[i];
            expected_args[i] = "";
            j ++;
        }
    }

    /* populate unrecognized_args */
    for (i = 0; i < cmd->n_hashes; i ++) {
        tmp = cmd->args[cmd->hashes[i]];
        
        while (tmp != NULL) {
            not_in_arr = 1;
            for (k = 0; k < len; k ++) {
                if (0 == strcmp(tmp->label, expected_args[k])) {
                    not_in_arr = 0;
                    break;
                }
            }
            if (not_in_arr) {
                unrecognized_args[m] = tmp->label;
                m ++;
            }
            tmp = tmp->next;
        }
    }

    missing_args_message = str_concat(missing_args, ARG_PREFIX, " ", j);
    unrecognized_args_message = str_concat(unrecognized_args, ARG_PREFIX, " ", m);

    if (!opt_args && NULL != missing_args_message) {       // if opt_args is false --> expected args are not optional
        raise_error(MISSING_ARGS_ERROR, 0, NULL, missing_args_message);
        valid = 0;
    }
    else if (opt_args && NULL != unrecognized_args_message) {
        raise_error(UNRECOGNIZED_ARGS_ERROR, 0, NULL, unrecognized_args_message);
        valid = 0;
    }

    if (NULL != missing_args_message) free(missing_args_message);
    if (NULL != unrecognized_args_message) free(unrecognized_args_message);
    return valid;
}


/* CHECK_ARGS() checks for unkown args; CHECK_REQ_ARGS() checks for required args missing */
cmd_retval execute_command(
    command *cmd, 
    raw_array *packets,
    shared_libs *libs, 
    custom_dissectors *custom_diss
) {
    cmd_retval retval = RET_UNKNOWN;

    if (is_command(cmd, ANALIZE_COMMAND)) {
        retval = RET_ANALIZE;
        if (CHECK_ARGS(cmd, ANALIZE_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_ANALIZE_ARGS)) execute_analize(cmd, packets, libs, custom_diss);
    }
    else if (is_command(cmd, DEVICES_LIST_COMMAND)) {
        retval = RET_DEVLIST;
        if (CHECK_ARGS(cmd, DEVICES_LIST_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_DEVICES_LIST_ARGS)) execute_devlist();
    }
    else if (is_command(cmd, DISSECTORS_COMMAND)) {
        retval = RET_DISSECTORS;
        if (CHECK_ARGS(cmd, DISSECTORS_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_DISSECTORS_ARGS)) execute_dissectors(cmd, libs, custom_diss);
    }
    else if (is_command(cmd, RESET_COMMAND)) {
        retval = RET_RESET;
        if (CHECK_ARGS(cmd, RESET_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_RESET_ARGS)) execute_reset(packets);
    }
    else if (is_command(cmd, PRINT_COMMAND)) {
        retval = RET_PRINT;
        if (CHECK_ARGS(cmd, PRINT_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_PRINT_ARGS)) execute_print(cmd, packets, libs, custom_diss);
    }
    else if (is_command(cmd, CLEAR_COMMAND)) {
        retval = RET_CLEAR;
        if (CHECK_ARGS(cmd, CLEAR_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_CLEAR_ARGS)) execute_clear();
    }
    else if (is_command(cmd, EXIT_COMMAND)) {
        retval = RET_EXIT;
        /*
        if (CHECK_ARGS(cmd, EXIT_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_EXIT_ARGS)) execute_exit();
        */
    }
    else if (is_command(cmd, SAVE_COMMAND)) {
        retval = RET_SAVE;
        if (CHECK_ARGS(cmd, SAVE_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_SAVE_ARGS)) execute_save(cmd, packets);
    }
    else if (is_command(cmd, HELP_COMMAND)) {
        retval = RET_HELP;
        if (CHECK_ARGS(cmd, HELP_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_HELP_ARGS)) execute_help();
    }
    return retval;
}