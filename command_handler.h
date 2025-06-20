#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <stddef.h>

#include "utils/command.h"
#include "utils/shared_lib.h"
#include "utils/custom_dissectors.h"
#include "utils/raw_array.h"

#define ANALIZE_COMMAND             "analize"
#define DEVICES_LIST_COMMAND        "devlist"
#define PROTOCOLS_COMMAND           "protocols"
#define DISSECTORS_COMMAND          "dissectors"
#define RESET_COMMAND               "reset"         /* erase currently stored packets (if command analize alredy run) */
#define PRINT_COMMAND               "print"         /* print the packets as done in analize (without re-analizing the net) */
#define CLEAR_COMMAND               "clear"         /* clean screen */
#define EXIT_COMMAND                "exit"
#define SAVE_COMMAND                "save"
#define HELP_COMMAND                "help"

#define ANALIZE_COMMAND_DESC        "Scan incoming and outgoing network traffic for a specific device"
#define DEVICES_LIST_COMMAND_DESC   "Retrieve a list of all available devices for scanning"
#define PROTOCOLS_COMMAND_DESC      "Retrieve a list of supported protocols"
#define DISSECTORS_COMMAND_DESC     "Manage custom dissectors"
#define RESET_COMMAND_DESC          "Reset stored packets"
#define PRINT_COMMAND_DESC          "Display detailed information for a specific packet"
#define CLEAR_COMMAND_DESC          "Clear screen output"
#define SAVE_COMMAND_DESC           "Save scanned packets in a .pcap file"
#define EXIT_COMMAND_DESC           "Exit program"

/*
    ATTENTION:
        Arguments must be alphanumeric because of the way the "_check_prefix_validity" function is implemented.
*/
#define NONE_ARG                    ""
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
#define PACKET_NUM_ARG              "pktnum"  /* show packet number next to packet */
#define READ_FILE_ARG               "r"
#define WRITE_FILE_ARG              "w"
#define DEST_FILE_ARG               "to"  /* (e.g. used to select the file where the scanned packets will be saved "save -to "dummy.pcap"") */
#define DISSECTOR_LIST_ARG          "list"
#define ADD_DISSECTOR_ARG           "add"
#define ACTIVATE_LIB_ARG            "on"
#define DEACTIVATE_LIB_ARG          "off"
#define PROTO_TABLES_ARG            "tables"
#define PROTO_FROM_ARG              "from"
#define PROTO_SEARCH_ARG            "search"

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
#define DISSECTOR_LIST_ARG_DESC     "Show custom dissectors list"
#define ADD_DISSECTOR_ARG_DESC      "Add custom dissector"
#define ACTIVATE_LIB_ARG_DESC       "Activate custom dissector (empty = all)"
#define DEACTIVATE_LIB_ARG_DESC     "Deactivate custom dissector (empty = all)"
#define PROTO_TABLES_ARG_DESC       "Show supported protocol tables"
#define PROTO_FROM_ARG_DESC         "Show supported protocols from the specified tables"
#define PROTO_SEARCH_ARG_DESC       "Looks up a protocol by number. If '" PROTO_FROM_ARG "' is set, the search is limited to the specified tables"

#define NUMBER_ARG_EG               ARG_PREFIX NUMBER_ARG               " 7"
#define FILTER_ARG_EG               ARG_PREFIX FILTER_ARG               " \"tcp port 80\""
#define DEVICE_ARG_EG               ARG_PREFIX DEVICE_ARG               " \"device-name\""
#define OUTPUT_FORMAT_ARG_EG        ARG_PREFIX OUTPUT_FORMAT_ARG        " \"art\""
#define READ_FILE_ARG_EG            ARG_PREFIX READ_FILE_ARG            " \"file-7.pcap\""
#define WRITE_FILE_ARG_EG           ARG_PREFIX WRITE_FILE_ARG           " \"file-7.pcap\""
#define DEST_FILE_ARG_EG            ARG_PREFIX DEST_FILE_ARG            " \"file-7.pcap\""
#define ADD_DISSECTOR_ARG_EG        ARG_PREFIX ADD_DISSECTOR_ARG        " \"/dir/file-7.so" STRINGS_SEPARATOR " /dir/file-8.so\""
#define ACTIVATE_LIB_ARG_EG         ARG_PREFIX ACTIVATE_LIB_ARG         " \"file-7.so" STRINGS_SEPARATOR " file-8.so\""
#define DEACTIVATE_LIB_ARG_EG       ARG_PREFIX DEACTIVATE_LIB_ARG       " \"file-7.so" STRINGS_SEPARATOR " file-8.so\""
#define PROTO_FROM_ARG_EG           ARG_PREFIX PROTO_FROM_ARG           " \"dlt_protos" STRINGS_SEPARATOR " ethertypes\"" 

#define OUTPUT_ARG_VAL_STD          "std"
#define OUTPUT_ARG_VAL_RAW          "raw"
#define OUTPUT_ARG_VAL_ART          "art"

/* #define {COMMAND_NAME}_ARGS   ARG1, ARG2, ..., ARGN */
/* 
    (e.g.) EXIT_ARGS are ALL the args possible for the command EXIT. 
    The REQUIRED_EXIT_ARGS are the obligatory args required for the cmd to run
*/
                   
#define ANALIZE_ARGS                NUMBER_ARG, FILTER_ARG, DEVICE_ARG, DATALINK_HDR_ARG, NETWORK_HDR_ARG, TRANSPORT_HDR_ARG, APPLICATION_HDR_ARG, NO_PROM_ARG, NO_TIMESTAMP_ARG, NO_PROTOCOL_NAME_ARG, PACKET_NUM_ARG, READ_FILE_ARG, WRITE_FILE_ARG
#define REQUIRED_ANALIZE_ARGS       NONE_ARG

#define DEVICES_LIST_ARGS           NONE_ARG
#define REQUIRED_DEVICES_LIST_ARGS  NONE_ARG

#define PROTOCOLS_ARGS              PROTO_TABLES_ARG, PROTO_FROM_ARG, PROTO_SEARCH_ARG
#define REQUIRED_PROTOCOLS_ARGS     NONE_ARG

#define DISSECTORS_ARGS             DISSECTOR_LIST_ARG, ADD_DISSECTOR_ARG, ACTIVATE_LIB_ARG, DEACTIVATE_LIB_ARG
#define REQUIRED_DISSECTORS_ARGS    NONE_ARG

#define RESET_ARGS                  NONE_ARG
#define REQUIRED_RESET_ARGS         NONE_ARG

#define PRINT_ARGS                  NUMBER_ARG, DATALINK_HDR_ARG, NETWORK_HDR_ARG, TRANSPORT_HDR_ARG, APPLICATION_HDR_ARG, NO_TIMESTAMP_ARG, NO_PROTOCOL_NAME_ARG, OUTPUT_FORMAT_ARG, PACKET_NUM_ARG
#define REQUIRED_PRINT_ARGS         NONE_ARG

#define CLEAR_ARGS                  NONE_ARG
#define REQUIRED_CLEAR_ARGS         NONE_ARG

#define EXIT_ARGS                   NONE_ARG
#define REQUIRED_EXIT_ARGS          NONE_ARG

#define SAVE_ARGS                   NUMBER_ARG, DEST_FILE_ARG
#define REQUIRED_SAVE_ARGS          DEST_FILE_ARG

#define HELP_ARGS                   NONE_ARG
#define REQUIRED_HELP_ARGS          NONE_ARG

typedef enum {
    RET_NONE,  /* when the command is known but has undefined retval (it doesn't trigger UNKOWN_COMMAND_ERROR, skips iteration instead) */
    RET_UNKNOWN,  /* when the command is unknown */
    RET_ANALIZE,
    RET_DEVLIST,
    RET_PROTOCOLS,
    RET_DISSECTORS,
    RET_RESET,
    RET_PRINT,
    RET_CLEAR,
    RET_EXIT,
    RET_SAVE,
    RET_HELP
} cmd_retval;


int create_cmd_from_str(command *cmd, char *str, const size_t len);
int is_valid(command *cmd, int opt_args, const char **expected_args, size_t len);
cmd_retval execute_command(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_diss);

#endif