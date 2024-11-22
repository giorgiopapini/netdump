#include "exit.h"
#include "../utils/packet.h"
#include <stdlib.h>

void execute_exit(command *cmd, raw_array *packets) {
    reset_cmd(cmd);
    reset_arr(packets, destroy_packet);
    /* The OS deallocates the remaining heap allocated memory that has not alredy been freed (such as circular_list history) */
    exit(EXIT_SUCCESS);
}