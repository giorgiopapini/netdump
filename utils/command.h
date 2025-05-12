#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

#define MAX_HASHES 128
#define MAX_ARGS 256


typedef struct arg {
    char *label;
    char *val;    /* Saved as string because the input is a string. Then should be correctly converted to the right type at runtime */
    struct arg *next;
} arg;

typedef struct command {
    size_t n_hashes;
    char *label;
    size_t hashes[MAX_HASHES];
    arg *args[MAX_HASHES];  /* create an hash function, *args should be an hash map (O(1) read time complexity) */
} command;


arg * create_arg_from_token(char *token);
int add_arg_from_token(command *cmd, char *temp, size_t *args_num);
unsigned long djb2_hash(const char *str);
void add_arg(command *cmd, arg *arg);
arg *get_arg(command *cmd, const char *label);
char *get_raw_val(command *cmd, const char *label);

int is_command(command *cmd, const char *command);
void reset_cmd(command *cmd);

#endif