#include "analize.h"
#include "../utils/string_utils.h"
#include "../analizer.h"

void execute_analize(command *cmd, raw_array *packets) {
    int pkt_num = -1;
    int tmp = str_to_num(get_raw_val(cmd, QUANTITY_ARG));
    char *filter_exp = get_raw_val(cmd, FILTER_ARG);

    /* if -n not provided (or -n value not set) returns 0. Analizing 0 packets doesn't make sense, so assume to scan to infinity */
    if (0 != tmp) pkt_num = tmp;

    sniff_packets(packets, pkt_num, filter_exp);
}
