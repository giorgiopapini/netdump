#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "buffer.h"
#include "utils/raw_array.h"

#define ANALIZE_COMMAND     "analize"
#define RESET_COMMAND       "reset"         /* erase currently stored packets (if command analize alredy run) */
#define PRINT_COMMAND       "print"         /* print the packets as done in analize (without re-analizing the net) */
#define VISUALIZE_COMMAND   "visualize"     /* nice formatting for specific packets */
#define CLEAR_COMMAND       "clear"         /* clean screen */
#define EXIT_COMMAND        "exit"
#define HELP_COMMAND        "help"

#define MAX_ARGS 128

typedef struct arg {
    char *label;
    char *val;    /* Saved as string because the input is a string. Then should be correctly converted to the right type at runtime */
    struct arg *next;
} arg;

typedef struct command {
    char *label;
    arg *args[MAX_ARGS];  /* create an hash function, *args should be an hash map (O(1) read time complexity) */
} command;

arg * create_arg_from_token(char *token);
void create_cmd_from_buff(command *cmd, buffer *buff);
unsigned long djb2_hash(char *str);
void add_arg(command *cmd, arg *arg);
arg *get_arg(command *cmd, char *label);

int is_command(command *cmd, const char *command);
int execute_command(command *cmd, raw_array *packets);
void reset_cmd(command *cmd);

#endif