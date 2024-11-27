#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "utils/buffer.h"
#include "utils/raw_array.h"
#include "utils/circular_linked_list.h"

#define ANALIZE_COMMAND     "analize"
#define RESET_COMMAND       "reset"         /* erase currently stored packets (if command analize alredy run) */
#define PRINT_COMMAND       "print"         /* print the packets as done in analize (without re-analizing the net) */
#define VISUALIZE_COMMAND   "visualize"     /* nice formatting for specific packets */
#define CLEAR_COMMAND       "clear"         /* clean screen */
#define EXIT_COMMAND        "exit"
#define HELP_COMMAND        "help"

#define ARG_PREFIX          "-"
#define QUANTITY_ARG        "n"
#define FILTER_ARG          "filter"
#define DATALINK_HDR_ARG    "e"  /* show datalink header */
#define NETWORK_HDR_ARG     "nnet" /* not net (set if the network layer should not be printed) */
#define NO_PROM_ARG         "p"  /* turn off promiscuos mode */
#define NO_TIMESTAMP_ARG    "t"  /* do not show formatted time next to packet */
#define PACKET_NUM_ARG      "#"  /* show packet number next to packet */
#define CMD_ARG             "cmd"

/* #define {COMMAND_NAME}_ARGS   ARG1, ARG2, ..., ARGN */
/* 
    (e.g.) EXIT_ARGS are ALL the args possible for the command EXIT. 
    The REQUIRED_EXIT_ARGS are the obligatory args required for the cmd to run
*/

#define EXIT_ARGS          
#define REQUIRED_EXIT_ARGS                   
#define ANALIZE_ARGS                QUANTITY_ARG, FILTER_ARG, DATALINK_HDR_ARG, NETWORK_HDR_ARG, NO_PROM_ARG, NO_TIMESTAMP_ARG, PACKET_NUM_ARG
#define REQUIRED_ANALIZE_ARGS                
#define PRINT_ARGS                  QUANTITY_ARG, DATALINK_HDR_ARG, NETWORK_HDR_ARG, NO_TIMESTAMP_ARG, PACKET_NUM_ARG
#define REQUIRED_PRINT_ARGS         
#define RESET_ARGS         
#define REQUIRED_RESET_ARGS                  
#define EXECUTE_ARGS       
#define REQUIRED_EXECUTE_ARGS                
#define HELP_ARGS                   CMD_ARG
#define REQUIRED_HELP_ARGS          

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
int create_cmd_from_buff(command *cmd, buffer *buff);
unsigned long djb2_hash(char *str);
void add_arg(command *cmd, arg *arg);
arg *get_arg(command *cmd, char *label);
char *get_raw_val(command *cmd, char *label);

int is_command(command *cmd, const char *command);
int is_valid(command *cmd, int opt_args, char **expected_args, size_t len);
int execute_command(command *cmd, raw_array *packets, circular_list *history);
void reset_cmd(command *cmd);


#endif