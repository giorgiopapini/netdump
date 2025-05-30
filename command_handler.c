#include "command_handler.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "status_handler.h"
#include "utils/string_utils.h"
#include "commands/analize.h"
#include "commands/devlist.h"
#include "commands/protocols.h"
#include "commands/dissectors.h"
#include "commands/reset.h"
#include "commands/print.h"
#include "commands/clear.h"
#include "commands/save.h"
#include "commands/help.h"

#define CHECK_REQ_ARGS(cmd, ...)            (is_valid(cmd, 0, (const char *[]){__VA_ARGS__}, LEN(char *, __VA_ARGS__)))
#define CHECK_ARGS(cmd, ...)                (is_valid(cmd, 1, (const char *[]){__VA_ARGS__}, LEN(char *, __VA_ARGS__)))

static int _word_count_matches(command *cmd, buffer *buff);
static int _check_prefix_validity(buffer *buff);
static void _load_str_val_pointers(char **start, char **end, char *str);
static void _load_cmd_label(command *cmd, buffer *buff);
static int _load_cmd_args(command *cmd, buffer *buff);


static int _word_count_matches(command *cmd, buffer *buff) {
    arg *tmp;
    size_t count = 0;
    size_t i;

    CHECK_NULL_RET(cmd, 1);
    CHECK_NULL_RET(buff, 1);
    CHECK_NULL_RET(buff->content, 1);

    if (NULL != cmd->label) count ++;

    for (i = 0; i < cmd->n_hashes; i ++) {
        tmp = cmd->args[cmd->hashes[i]];
        while (tmp != NULL) {
            count ++;
            if (NULL != tmp->val) count += count_words(tmp->val, strlen(tmp->val));
            tmp = tmp->next;
        }
    }
    return !(count == count_words(buff->content, buff->len));
    /* success = 0, failure = 1 (or !0 in general) */
}

int _check_prefix_validity(buffer *buff) {  /* "Fil-1.pcap" is not correctly interpreted */
    char *start_substr;
    size_t prefix_len;
    ptrdiff_t delta;

    CHECK_NULL_RET(buff, 1);
    CHECK_NULL_RET(buff->content, 1);
    CHECK_NULL_RET(ARG_PREFIX, 0);

    prefix_len = strlen(ARG_PREFIX);

    /* if words count > 0 than the words after the first should be arguments,
    otherwise it means the user executed command without args */
    if (1 >= count_words(buff->content, buff->len)) return 0;

    start_substr = strstr(buff->content, ARG_PREFIX);
    while (NULL != start_substr) {
        delta = start_substr - buff->content;
        if (delta > 0 && (size_t)delta >= buff->len - 1) break;

        if (delta >= 0) {
            if (0 == strncmp(start_substr, ARG_PREFIX, prefix_len) && ((size_t)delta + prefix_len) < buff->len) {
                if (
                    !isdigit(buff->content[(size_t)delta + prefix_len]) &&
                    !isalpha(buff->content[(size_t)delta + prefix_len])
                ) return 1;
            }
        }

        start_substr = strstr(start_substr + 1, ARG_PREFIX);
    }

    return 0;
}

static void _load_str_val_pointers(char **start, char **end, char *str) {
    /* loads starting and ending position of the string value of an arg like -r "tests-1.pcap" */
    char *tmp = NULL;
    
    CHECK_NULL_EXIT(start);
    CHECK_NULL_EXIT(end);
    CHECK_NULL_EXIT(str);
    CHECK_NULL_EXIT(ARG_STR_DELIMITER);
    
    *start = NULL;
    *end = NULL;

    tmp = strstr(str, ARG_STR_DELIMITER);
    if (NULL != tmp) {
        *start = tmp;
        tmp = strstr(tmp + strlen(ARG_STR_DELIMITER), ARG_STR_DELIMITER);
        if (NULL != tmp) *end = tmp;
    }
}

static void _load_cmd_label(command *cmd, buffer *buff) {
    /* cmd and buff should not be NULL */
    size_t i;
    
    for (i = 0; i < buff->len; i ++)
        if (' ' == buff->content[i]) break;

    cmd->label = malloc(i + 1);
    CHECK_NULL_EXIT(cmd->label);

    strncpy(cmd->label, buff->content, i);
    cmd->label[i] = '\0';
}

static int _load_cmd_args(command *cmd, buffer *buff) {
    char *temp_arg;
    char *temp_next_arg;
    char token[MAX_BUFFER_LEN];
    ptrdiff_t delta;
    ptrdiff_t token_delta;
    size_t copy_len;
    size_t args_num = 0;  /* should always be initialized */
    int status = 0;  /* if 0 means success and != 0 means failure */

    char *start_str_val = NULL;
    char *end_str_val = NULL;

    CHECK_NULL_EXIT(buff);
    CHECK_NULL_EXIT(ARG_PREFIX);

    _load_str_val_pointers(&start_str_val, &end_str_val, buff->content);

    temp_arg = strstr(buff->content, ARG_PREFIX);
    while (NULL != temp_arg) {
        delta = temp_arg - buff->content;
        if (delta > 0 && (size_t)delta >= buff->len - 1) break;
        
        /* if inside str delimiter than skip */
        temp_next_arg = strstr(temp_arg + strlen(ARG_PREFIX), ARG_PREFIX);
        if (NULL != start_str_val) {
            if (NULL != end_str_val) {
                while (temp_next_arg >= start_str_val && temp_next_arg <= end_str_val) {
                    temp_next_arg = strstr(temp_next_arg + strlen(ARG_PREFIX), ARG_PREFIX);
                }
            }
            else {
                raise_error(NON_CLOSING_STR_ERROR, 0, NULL, ARG_STR_DELIMITER);
                return 1;
            }
        }

        if (NULL == temp_next_arg)
            token_delta = (buff->content + buff->len) - temp_arg; 
        else token_delta = temp_next_arg - temp_arg;

        if (token_delta >= 0) {
            copy_len = (token_delta < MAX_BUFFER_LEN - 1) ? (size_t)token_delta : MAX_BUFFER_LEN - 1;
            strncpy(token, temp_arg, copy_len);
            token[copy_len] = '\0';

            /* if token = "--arg 100" --> the actual arg string will be "arg 100" */
            status = add_arg_from_token(cmd, token + strlen(ARG_PREFIX), &args_num);
            if (0 != status) return 1;  /* if argument insertions failed, than exit the loop */
        }

        memset(token, '\0', MAX_BUFFER_LEN);
        temp_arg = temp_next_arg;
        /* reset token, find next temp_arg and go next iteration */
    }
    return 0;
}

int create_cmd_from_buff(command *cmd, buffer *buff) {
    int status = 0;

    CHECK_NULL_RET(cmd, 1);
    CHECK_NULL_RET(buff, 1);
    CHECK_NULL_EXIT(ARG_PREFIX);

    /* if there is <command> <arg> without the PREFIX separator than raise a formatting error */
    if (0 != _check_prefix_validity(buff)) {
        raise_error(WRONG_OPTIONS_FORMAT_ERROR, 0, NULL);
        return 1;
    }

    _load_cmd_label(cmd, buff);  /* always true, it just copy the first word from buff->content to cmd->label */
    status = _load_cmd_args(cmd, buff);
    if (0 != status) return status;
    /* early exit to avoid printing another error message */

    /* 0 if equal, !0 if not equal */
    status = _word_count_matches(cmd, buff);
    if (0 != status) raise_error(WRONG_OPTIONS_FORMAT_ERROR, 0, NULL);
    return status;
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

    CHECK_NULL_EXIT(cmd);
    CHECK_NULL_EXIT(expected_args);

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
        CHECK_NULL_EXIT(expected_args[0]);
        CHECK_NULL_EXIT(NONE_ARG);
        if (0 < len && 0 == strcmp(expected_args[0], NONE_ARG)) valid = 1;
        else {
            raise_error(MISSING_ARGS_ERROR, 0, NULL, missing_args_message);
            valid = 0;
        }
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
    else if (is_command(cmd, PROTOCOLS_COMMAND)) {
        retval = RET_PROTOLIST;
        if (CHECK_ARGS(cmd, PROTOLIST_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_PROTOLIST_ARGS)) execute_protocols(cmd);
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
        retval = RET_NONE;
        if (CHECK_ARGS(cmd, CLEAR_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_CLEAR_ARGS)) retval = RET_EXIT;
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