#include "print.h"
#include "../utils/string_utils.h"
#include "../utils/packet.h"
#include "../protocols/protocol_handler.h"

void execute_print(command *cmd, raw_array *packets) {
    int pkt_num = str_to_num(get_raw_val(cmd, QUANTITY_ARG));
    packet *pkt = get(packets, pkt_num);  /* +1 because array starts at 0, but for the user the first packet is 1, not 0 */
    if (NULL != pkt) dissect_packet(cmd, pkt);
}