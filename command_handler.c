#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "status_handler.h"
#include "utils/string_utils.h"
#include "utils/formats.h"

#include "command_handler.h"
#include "commands/help.h"
#include "commands/exit.h"
#include "commands/analize.h"
#include "commands/devlist.h"
#include "commands/print.h"
#include "commands/visualize.h"
#include "commands/clear.h"
#include "commands/reset.h"

#define CHECK_REQ_ARGS(cmd, ...)            (is_valid(cmd, 0, (char*[]){__VA_ARGS__}, LEN(char*, __VA_ARGS__)))
#define CHECK_ARGS(cmd, ...)                (is_valid(cmd, 1, (char*[]){__VA_ARGS__}, LEN(char*, __VA_ARGS__)))

arg * create_arg_from_token(char *token) {
    /*  check if token ends with a whitespace (which is mandatory when multiple args exists, otherwise '-<label> <value>' wouldn't 
        be recognizable)    */

    int len = strlen(token);
    if (0 == len) return NULL;

    if (' ' == token[len - 1]) token[len - 1] = '\0';

    int token_len = strlen(token) + 1;  /* strlen() does NOT count null terminator (this is why the +1 is needed) */
    int label_len;
    int value_len;
    char copy[token_len];
    arg *new_arg = (arg *)malloc(sizeof(arg));
    if (NULL == new_arg) raise_error(NULL_POINTER, 1, NULL, "arg *new_arg", __FILE__);
    new_arg->label = NULL;
    new_arg->val = NULL;
    new_arg->next = NULL;
    
    memcpy(copy, token, token_len);

    label_len = find_word_len(token, 0);
    if (0 >= label_len) {
        raise_error(WRONG_OPTIONS_FORMAT_ERROR, 0, NULL);
        return NULL;
    }

    copy_str_n(&new_arg->label, token, label_len);
    
    value_len = strlen(token) - label_len;  /* it alredy includes the null terminator */
    if (0 < value_len) {
        new_arg->val = (char *)malloc(value_len);
        if (NULL == new_arg->val) raise_error(NULL_POINTER, 1, NULL, "new_arg->val", __FILE__);
        memset(new_arg->val, '\0', value_len);
        strcpy(new_arg->val, token + label_len + 1);    /* +1 needed to skip the first whitespace that separates -<key> to <value> */
    }
    else new_arg->val = NULL;

    return new_arg;
}

unsigned long djb2_hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    return hash % MAX_HASHES;
}

void add_arg(command *cmd, arg *new_arg) {
    unsigned long hash = djb2_hash(new_arg->label);

    if (NULL == cmd->args[hash]) {
        cmd->args[hash] = new_arg;
        cmd->hashes[cmd->n_hashes] = hash;  /* if hash is new, than add it to the hashes array (no duplicates) */
        cmd->n_hashes ++;
    }
    else {
        char *old_label = cmd->args[hash]->label;
        if (0 == strcmp(old_label, new_arg->label)) cmd->args[hash] = new_arg;
        else cmd->args[hash]->next = new_arg;  /* if alredy exist arg with hash=(x), than add new one to the linked list */
    }
}

arg * get_arg(command *cmd, char *label) {
    arg *res = cmd->args[djb2_hash(label)];
    while (NULL != res) {
        if (0 == strcmp(res->label, label)) return res;
        res = res->next;
    }
    return NULL;
}

char *get_raw_val(command *cmd, char *label) {
    arg *obt = get_arg(cmd, label);
    if (NULL == obt) return NULL;
    else if (NULL == obt->val) return NULL;
    return obt->val;
}

int check_compliance(buffer *buff) {
    int i;

    for (i = 1; i < buff->len; i ++) {
        /* if char at (i - 1) is not empty and is not the leading char (which may happen, because of the buffer trim func) */
        if (buff->content[i - 1] == ' ' && 0 != (i - 1)) {
            if (buff->content[i] == ARG_PREFIX[0]) return 1;
            else return 0;
        }
    }

    return 1;
}

int add_arg_from_token(command *cmd, char *temp, size_t *args_num) {
    arg *new_arg = create_arg_from_token(temp);
    if (NULL == new_arg) return 1;
    *args_num = *args_num + 1;

    if (MAX_ARGS < *args_num) {
        raise_error(TOO_MANY_ARGS, 0, NULL, MAX_ARGS);
        return 1;
    }
    add_arg(cmd, new_arg);
    return 0;
}

int create_cmd_from_buff(command *cmd, buffer *buff) {
    size_t args_num = 0;
    char temp[buff->len + 1];  /* including null terminator */
    int i, j;
    int str_arg_value = 0;  /* flag to check if " already showed up */
    int writing_arg = 0;  /* flag to check if ARG_PREFIX alredy showed up */
    int status = 0;

    /* if there is <command> <arg> without the PREFIX separator than raise a formatting error */
    if (!check_compliance(buff)) {
        raise_error(WRONG_OPTIONS_FORMAT_ERROR, 0, NULL);
        return 1;
    }
    if (NULL == ARG_PREFIX) raise_error(NULL_POINTER, 1, NULL, "ARG_PREFIX", __FILE__);

    cmd->label = NULL;
    for (i = 0, j = 0; i < buff->len; i ++, j ++) {
        temp[j] = buff->content[i];
        if (NULL == cmd->label) {
            if (j > 0 && ' ' == temp[j]) {
                temp[j] = '\0';
                cmd->label = (char *)malloc(j + 1);
                if (NULL == cmd->label) raise_error(NULL_POINTER, 1, NULL, "cmd->label", __FILE__);
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

    temp[j] = '\0';
    if (NULL == cmd->label) {
        cmd->label = (char *)malloc(j + 1);
        if (NULL == cmd->label) raise_error(NULL_POINTER, 1, NULL, "cmd->label", __FILE__);
        strcpy(cmd->label, temp);
    }
    else {
        if (strlen(temp) > 0) status = add_arg_from_token(cmd, temp, &args_num);
        if (status) return 1;
    }
    return 0;
}

int is_command(command *cmd, const char *command) {
    if (NULL == cmd->label || NULL == command) return 0;
    if (0 == strcmp(cmd->label, command)) return 1;
    return 0;
}

int is_valid(command *cmd, int opt_args, char **expected_args, size_t len) {
    arg *tmp = NULL;
    int valid = 1;
    int i;
    int j = 0;  /* will represent length of missing_args array */
    int k;  
    int m = 0;  /* will represent length of unrecognized_args array */

    char *missing_args[MAX_ARGS]; /* size_t len might represents the 'worst case length'?. Every arg misses so missing_args has length = len */
    char *missing_args_message = NULL;
    char *unrecognized_args[MAX_ARGS];
    char *unrecognized_args_message = NULL;

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
            int not_in_arr = 1;
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
int execute_command(command *cmd, raw_array *packets, circular_list *history) {
    if (is_command(cmd, EXIT_COMMAND)) {
        if (CHECK_ARGS(cmd, EXIT_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_EXIT_ARGS)) execute_exit(cmd, packets, history);
    }
    else if (is_command(cmd, ANALIZE_COMMAND)) {
        if (CHECK_ARGS(cmd, ANALIZE_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_ANALIZE_ARGS)) execute_analize(cmd, packets);
    }
    else if (is_command(cmd, DEVICES_LIST_COMMAND)) {
        if (CHECK_ARGS(cmd, DEVICES_LIST_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_DEVICES_LIST_ARGS)) execute_devlist(cmd);
    }
    else if (is_command(cmd, PRINT_COMMAND)) {
        if (CHECK_ARGS(cmd, PRINT_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_PRINT_ARGS)) execute_print(cmd, packets);
    }
    else if (is_command(cmd, VISUALIZE_COMMAND)) {
        if (CHECK_ARGS(cmd, VISUALIZE_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_VISUALIZE_ARGS)) execute_visualize(cmd, packets);
    }
    else if (is_command(cmd, RESET_COMMAND)) {
        if (CHECK_ARGS(cmd, RESET_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_RESET_ARGS)) execute_reset(cmd, packets);
    }
    else if (is_command(cmd, CLEAR_COMMAND)) {
        if (CHECK_ARGS(cmd, CLEAR_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_CLEAR_ARGS)) execute_clear(cmd);
    }
    else if (is_command(cmd, HELP_COMMAND)) {
        if (CHECK_ARGS(cmd, HELP_ARGS))
        if (CHECK_REQ_ARGS(cmd, REQUIRED_HELP_ARGS)) execute_help(cmd);
    }
    else return 1;

    return 0;
}

void reset_cmd(command *cmd) {
    arg *tmp;
    arg *curr = NULL;

    if (NULL == cmd) raise_error(NULL_POINTER, 0, NULL, "cmd", __FILE__);

    /* deallocate hashmap */
    for (int i = 0; i < cmd->n_hashes; i ++) {
        curr = cmd->args[cmd->hashes[i]];
        while (NULL != curr) {
            free(curr->label);
            free(curr->val);
            tmp = curr->next;
            free(curr);
            curr = tmp;
        }
        cmd->args[cmd->hashes[i]] = NULL;
    }
    /* erasing the hashes array is not needed. he array is overwritten for each command inserted */

    if (NULL != cmd->label) free(cmd->label);
    cmd->label = NULL;
    cmd->n_hashes = 0;
}