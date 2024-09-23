#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "analizer.h"
#include "command_handler.h"
#include "status_handler.h"
#include "utils/string_utils.h"


arg * create_arg_from_token(char *token) {
    arg *new_arg = (arg *)malloc(sizeof(arg));
    new_arg->label = NULL;
    new_arg->val = NULL;

    char copy[strlen(token) + 1];
    memcpy(copy, token, strlen(token) + 1);

    int label_len = find_word_len(token, 0);

    copy_str_n(&new_arg->label, token, label_len);
    
    printf("len: %d| label:%s\n",label_len, new_arg->label);

    /* IMPORTANT! value_len keeps track of leading and trailing white spaces. IT SHOULD NOT*/
    /* it should only keep track of the actual value length */
    
    int value_len = strlen(token) - label_len;
    new_arg->val = (char *)malloc(value_len);
    memset(new_arg->val, '\0', value_len);
    strcpy(new_arg->val, token + label_len);
    
    printf("value len: %d| value:%s\n", value_len, new_arg->val);

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