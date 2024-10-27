#include "print.h"
#include "../utils/string_utils.h"

void execute_print(command *cmd, raw_array *packets) {
    int pkt_num = str_to_num(get_raw_val(cmd, QUANTITY_ARG));
    void *pkt = get(packets, pkt_num);
}