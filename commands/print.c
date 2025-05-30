#include "print.h"

#include <stddef.h>

#include "../protocol_handler.h"
#include "../status_handler.h"
#include "../utils/packet.h"
#include "../utils/string_utils.h"
#include "../command_handler.h"
#include "../utils/custom_dissectors.h"
#include "../utils/shared_lib.h"
#include "../utils/command.h"
#include "../utils/raw_array.h"


void execute_print(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_diss) {
    packet *pkt;
    size_t pkt_num;
    long raw_pkt_num = str_to_long(get_raw_val(cmd, NUMBER_ARG));
    
    if (0 > raw_pkt_num) {
        raise_error(NEGATIVE_BUFFER_INDEX, 0, NULL, raw_pkt_num); 
        return;
    }
    else pkt_num = (size_t)raw_pkt_num;

    pkt = get(packets, (0 == pkt_num ? 1 : pkt_num));  /* if pkt_num = 0, index would be -1. This is prevented */
    if (NULL != pkt) dissect_packet(cmd, pkt, libs, custom_diss);
}