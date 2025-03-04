#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "utils/buffer.h"
#include "utils/raw_array.h"
#include "utils/circular_linked_list.h"

#define ANALIZE_COMMAND             "analize"
#define DEVICES_LIST_COMMAND        "devlist"
#define RESET_COMMAND               "reset"         /* erase currently stored packets (if command analize alredy run) */
#define PRINT_COMMAND               "print"         /* print the packets as done in analize (without re-analizing the net) */
#define CLEAR_COMMAND               "clear"         /* clean screen */
#define EXIT_COMMAND                "exit"
#define SAVE_COMMAND                "save"
#define HELP_COMMAND                "help"

#define ANALIZE_COMMAND_DESC        "Scan incoming and outgoing network traffic for a specific device"
#define DEVICES_LIST_COMMAND_DESC   "Retrieve a list of all available devices for scanning"
#define RESET_COMMAND_DESC          "Reset stored packets"
#define PRINT_COMMAND_DESC          "Display detailed information for a specific packet"
#define CLEAR_COMMAND_DESC          "Clear screen output"
#define SAVE_COMMAND_DESC           "Save scanned packets in a .pcap file"
#define EXIT_COMMAND_DESC           "Exit program"

#define ARG_PREFIX                  "-"
#define NUMBER_ARG                  "n"
#define FILTER_ARG                  "filter"
#define DEVICE_ARG                  "dev"
#define DATALINK_HDR_ARG            "e"  /* show datalink header */
#define TRANSPORT_HDR_ARG           "t"  /* shot transport header */
#define NETWORK_HDR_ARG             "nnet" /* not net (set if the network layer should not be printed) */
#define APPLICATION_HDR_ARG         "a"
#define NO_PROM_ARG                 "nprom"  /* turn off promiscuos mode */
#define NO_TIMESTAMP_ARG            "ntime"  /* do not show formatted time next to packet */
#define NO_PROTOCOL_NAME_ARG        "npname"  /* do not show the protocol name during the printing */
#define OUTPUT_FORMAT_ARG           "output"  /* choose the output format (std / raw / art) */
#define PACKET_NUM_ARG              "#"  /* show packet number next to packet */
#define READ_FILE_ARG               "r"
#define WRITE_FILE_ARG              "w"
#define DEST_FILE_ARG               "to"  /* (e.g. used to select the file where the scanned packets will be saved "save -to "dummy.pcap"") */

#define NUMBER_ARG_DESC             "Choose a packet by its scanning order number"
#define PACKET_AMOUNT_ARG_DESC      "Set a packet scanning limit"
#define FILTER_ARG_DESC             "Filter packets (based upon libpcap's filter function)"
#define DEVICE_ARG_DESC             "Select an avaliable device for scanning network traffic"
#define DATALINK_HDR_ARG_DESC       "Show datalink layer protocol header"
#define NETWORK_HDR_ARG_DESC        "Don't show network layer protocol header"
#define TRANSPORT_HDR_ARG_DESC      "Show transport layer protocol header"
#define APPLICATION_HDR_ARG_DESC    "Show application layer protocol header"
#define NO_PROM_ARG_DESC            "Turn off libpcap promiscuos mode"
#define NO_TIMESTAMP_ARG_DESC       "Hide timestamp"
#define NO_PROTOCOL_NAME_ARG_DESC   "Hide protocol name in the output"
#define OUTPUT_FORMAT_ARG_DESC      "Select output format (" OUTPUT_ARG_VAL_STD " / " OUTPUT_ARG_VAL_RAW " / " OUTPUT_ARG_VAL_ART ")"
#define PACKET_NUM_ARG_DESC         "Show packet number"
#define READ_FILE_ARG_DESC          "Read a .pcap file"
#define WRITE_FILE_ARG_DESC         "Write a .pcap file with all the scanned packets"
#define DEST_FILE_ARG_DESC          "Specify the destination file for saving scanned data"

#define NUMBER_ARG_EG           ARG_PREFIX NUMBER_ARG               " 7"
#define FILTER_ARG_EG           ARG_PREFIX FILTER_ARG               " \"tcp port 80\""
#define DEVICE_ARG_EG           ARG_PREFIX DEVICE_ARG               " \"device-name\""
#define OUTPUT_FORMAT_ARG_EG    ARG_PREFIX OUTPUT_FORMAT_ARG        " \"art\""
#define READ_FILE_ARG_EG        ARG_PREFIX READ_FILE_ARG            " \"file-7.pcap\""
#define WRITE_FILE_ARG_EG       ARG_PREFIX WRITE_FILE_ARG           " \"file-7.pcap\""
#define DEST_FILE_ARG_EG        ARG_PREFIX DEST_FILE_ARG            " \"file-7.pcap\""

#define OUTPUT_ARG_VAL_STD          "std"
#define OUTPUT_ARG_VAL_RAW          "raw"
#define OUTPUT_ARG_VAL_ART          "art"

#define COMMAND_FORMAT "<command> " ARG_PREFIX "<arg> <value>"
#define STRING_DELIMITER '"'

/* #define {COMMAND_NAME}_ARGS   ARG1, ARG2, ..., ARGN */
/* 
    (e.g.) EXIT_ARGS are ALL the args possible for the command EXIT. 
    The REQUIRED_EXIT_ARGS are the obligatory args required for the cmd to run
*/
                   
#define ANALIZE_ARGS                NUMBER_ARG, FILTER_ARG, DEVICE_ARG, DATALINK_HDR_ARG, NETWORK_HDR_ARG, TRANSPORT_HDR_ARG, APPLICATION_HDR_ARG, NO_PROM_ARG, NO_TIMESTAMP_ARG, NO_PROTOCOL_NAME_ARG, PACKET_NUM_ARG, READ_FILE_ARG, WRITE_FILE_ARG
#define REQUIRED_ANALIZE_ARGS

#define DEVICES_LIST_ARGS           
#define REQUIRED_DEVICES_LIST_ARGS  

#define RESET_ARGS
#define REQUIRED_RESET_ARGS

#define PRINT_ARGS                  NUMBER_ARG, DATALINK_HDR_ARG, NETWORK_HDR_ARG, TRANSPORT_HDR_ARG, APPLICATION_HDR_ARG, NO_TIMESTAMP_ARG, NO_PROTOCOL_NAME_ARG, OUTPUT_FORMAT_ARG, PACKET_NUM_ARG
#define REQUIRED_PRINT_ARGS

#define CLEAR_ARGS
#define REQUIRED_CLEAR_ARGS

#define EXIT_ARGS          
#define REQUIRED_EXIT_ARGS

#define SAVE_ARGS                   NUMBER_ARG, DEST_FILE_ARG
#define REQUIRED_SAVE_ARGS          DEST_FILE_ARG

#define HELP_ARGS                   
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