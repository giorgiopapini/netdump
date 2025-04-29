#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "../status_handler.h"
#include "string_utils.h"

arg * create_arg_from_token(char *token) {
    /*  check if token ends with a whitespace (which is mandatory when multiple args exists, otherwise '-<label> <value>' wouldn't 
        be recognizable)    */

    size_t len = strlen(token);
    if (0 == len) return NULL;

    if (' ' == token[len - 1]) token[len - 1] = '\0';

    size_t token_len = strlen(token) + 1;  /* strlen() does NOT count null terminator (this is why the +1 is needed) */
    size_t label_len;
    size_t value_len;
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

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % MAX_HASHES;
}

void add_arg(command *cmd, arg *new_arg) {
    char *old_label;
    unsigned long hash = djb2_hash(new_arg->label);

    if (NULL == cmd->args[hash]) {
        cmd->args[hash] = new_arg;
        cmd->hashes[cmd->n_hashes] = hash;  /* if hash is new, than add it to the hashes array (no duplicates) */
        cmd->n_hashes ++;
    }
    else {
        old_label = cmd->args[hash]->label;
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

int is_command(command *cmd, const char *command) {
    if (NULL == cmd->label || NULL == command) return 0;
    if (0 == strcmp(cmd->label, command)) return 1;
    return 0;
}

void reset_cmd(command *cmd) {
    arg *tmp;
    arg *curr = NULL;
    size_t i;

    if (NULL == cmd) raise_error(NULL_POINTER, 0, NULL, "cmd", __FILE__);

    /* deallocate hashmap */
    for (i = 0; i < cmd->n_hashes; i ++) {
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