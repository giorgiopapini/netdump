#include "print.h"
#include "../utils/string_utils.h"
#include "../utils/packet.h"
#include "../protocol_handler.h"

void execute_print(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_dissectors) {
    int pkt_num = str_to_num(get_raw_val(cmd, NUMBER_ARG));
    packet *pkt = get(packets, (0 == pkt_num ? 1 : pkt_num));  /* if pkt_num = 0, index would be -1. This is prevented */
    if (NULL != pkt) dissect_packet(cmd, pkt, libs, custom_dissectors);
}