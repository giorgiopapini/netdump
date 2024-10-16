#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "analizer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/string_utils.h"


arg * create_arg_from_token(char *token) {
    /*  check if token ends with a whitespace (which is mandatory when multiple args exists, otherwise '-<label> <value>' wouldn't 
        be recognizable)    */

    int len = strlen(token);
    if (' ' == token[len - 1]) token[len - 1] = '\0';

    int token_len = strlen(token) + 1;  /* strlen() does NOT count null terminator (this is why the +1 is needed) */
    int label_len;
    int value_len;
    char copy[token_len];
    arg *new_arg = (arg *)malloc(sizeof(arg));
    if (NULL == new_arg) raise(NULL_POINTER, 1, NULL, "arg *new_arg", __FILE__);
    new_arg->label = NULL;
    new_arg->val = NULL;
    new_arg->next = NULL;
    
    memcpy(copy, token, token_len);

    label_len = find_word_len(token, 0);
    if (0 >= label_len) raise(WRONG_OPTIONS_FORMAT_ERROR, 0, NULL);

    copy_str_n(&new_arg->label, token, label_len);
    
    value_len = strlen(token) - label_len;  /* it alredy includes the null terminator */
    if (0 < value_len) {
        new_arg->val = (char *)malloc(value_len);
        if (NULL == new_arg->val) raise(NULL_POINTER, 1, NULL, "new_arg->val", __FILE__);
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
    return hash % MAX_ARGS;
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

void create_cmd_from_buff(command *cmd, buffer *buff) {
    char copy[buff->len + 1];   /* including null terminator */
    memcpy(copy, buff->content, buff->len + 1);
    char *token = strtok(copy, " ");
    while (NULL != token) {
        if (NULL == cmd->label) {   /* first token is the command label. Other ones are arguments */
            cmd->label = (char *)malloc(strlen(token));
            strcpy(cmd->label, token);
            token = strtok(NULL, "-");  /* skip command token once read */
        }
        else {
            arg *new_arg = create_arg_from_token(token);
            add_arg(cmd, new_arg);
            token = strtok(NULL, "-");
        }
    }
}

int is_command(command *cmd, const char *command) {
    if (0 == strcmp(cmd->label, command)) return 1;
    return 0;
}


/* commands execution definitions */

void execute_exit(command *cmd, raw_array *packets) {
    reset_cmd(cmd);
    reset_arr(packets);
    exit(EXIT_SUCCESS);
}

void execute_analize(command *cmd, raw_array *packets) {
    int pkt_num = -1;
    int tmp = str_to_num(get_raw_val(cmd, QUANTITY_ARG));

    /* if -n not provided (or -n value not set) returns 0. Analizing 0 packets doesn't make sense, so assume to scan to infinity */
    if (0 != tmp) pkt_num = tmp;

    /* when to allocate memory space for values inside packets array? How to manage unlimited packets retrieve? */
    sniff_packets(packets, pkt_num);
}

void execute_print(command *cmd, raw_array *packets) {
    int pkt_num = str_to_num(get_raw_val(cmd, QUANTITY_ARG));
    void *pkt = get(packets, pkt_num);
}

void execute_reset(command *cmd, raw_array *packets) {
    if (0 == packets->allocated) print_success_msg(ARRAY_EMPTY_SUCCESS);
    else {
        reset_arr(packets);
        print_success_msg(ARRAY_RESET_SUCCESS);
    }
}

void execute_clear(command *cmd) {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #elif defined(__linux__) || defined(__APPLE__)
        system("clear");
    #else
        raise(COMMAND_NOT_SUPPORTED_ERROR, 0, CLEAN_COMMAND);
    #endif
}

void execute_help() {
    printf("HELP LIST\n");
}

int execute_command(command *cmd, raw_array *packets) {
    if (is_command(cmd, EXIT_COMMAND)) execute_exit(cmd, packets);
    else if (is_command(cmd, ANALIZE_COMMAND)) {
        execute_analize(cmd, packets);
        return 0;
    }
    else if (is_command(cmd, PRINT_COMMAND)) {
        execute_print(cmd, packets);
        return 0;
    }
    else if (is_command(cmd, RESET_COMMAND)) {
        execute_reset(cmd, packets);
        return 0;
    }
    else if (is_command(cmd, CLEAR_COMMAND)) {
        execute_clear(cmd);
        return 0;
    }
    else if (is_command(cmd, HELP_COMMAND)) {
        execute_help();
        return 0;
    }

    return 1;
}

void reset_cmd(command *cmd) {
    arg *tmp;
    arg *curr = NULL;

    if (NULL == cmd) raise(NULL_POINTER, 0, NULL, "cmd", __FILE__);

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