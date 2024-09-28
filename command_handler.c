#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "analizer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/string_utils.h"


arg * create_arg_from_token(char *token) {
    int token_len = strlen(token) + 1;  /* strlen() does NOT count null terminator (this is why the +1 is needed) */
    int label_len;
    int value_len;
    char copy[token_len];
    arg *new_arg = (arg *)malloc(sizeof(arg));
    if (NULL == new_arg) raise(NULL_POINTER, 1, "arg *new_arg", __FILE__);
    new_arg->label = NULL;
    new_arg->val = NULL;
    
    memcpy(copy, token, token_len);

    label_len = find_word_len(token, 0);
    if (0 >= label_len) raise(WRONG_OPTIONS_FORMAT_ERROR, 0);

    copy_str_n(&new_arg->label, token, label_len);
    
    value_len = strlen(token) - label_len;  /* it alredy includes the null terminator */
    new_arg->val = (char *)malloc(value_len);
    if (NULL == new_arg->val) raise(NULL_POINTER, 1, "new_arg->val", __FILE__);

    memset(new_arg->val, '\0', value_len);
    strcpy(new_arg->val, token + label_len + 1);    /* +1 needed to skip the first whitespace that separates -<key> to <value> */

    return new_arg;
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
            /* create arg (add key and value here before strtok again) */
            arg *new_arg = create_arg_from_token(token);
            token = strtok(NULL, "-");
        }
    }
}

int is_command(command *cmd, const char *command) {
    if (0 == strcmp(cmd->label, command)) return 1;
    return 0;
}

int execute_command(command *cmd, raw_array *packets) {
    if (is_command(cmd, EXIT_COMMAND)) {
        reset_cmd(cmd);
        reset_arr(packets);
        exit(EXIT_SUCCESS);
    }
    else if (is_command(cmd, ANALIZE_COMMAND)) {
        /* when to allocate memory space for values inside packets array? How to manage unlimited packets retrieve? */
        sniff_packets(packets, 60);
        return 0;
    }
    else if (is_command(cmd, PRINT_COMMAND)) {
        void *pkt = get_pkt(packets, 80);
        return 0;
    }
    else if (is_command(cmd, RESET_COMMAND)) {
        if (0 == packets->allocated) print_success_msg(ARRAY_EMPTY_SUCCESS);
        else {
            reset_arr(packets);
            print_success_msg(ARRAY_RESET_SUCCESS);
        }
        return 0;
    }
    else if (is_command(cmd, CLEAR_COMMAND)) {
        #if defined(_WIN32) || defined(_WIN64)
            system("cls");
        #elif defined(__linux__) || defined(__APPLE__)
            system("clear");
        #else
            raise(COMMAND_NOT_SUPPORTED_ERROR, 0, CLEAN_COMMAND);
        #endif
        return 0;
    }
    else if (is_command(cmd, HELP_COMMAND)) {
        printf("HELP LIST\n");
        return 0;
    }
    
    return 1;
}

void reset_cmd(command *cmd) {
    if (NULL == cmd) raise(NULL_POINTER, 0, "cmd", __FILE__);

    if (NULL != cmd->label) free(cmd->label);
    cmd->label = NULL;

    /* IMPORTANT! --> Deallocate the future args implementation */
}