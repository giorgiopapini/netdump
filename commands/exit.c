#include "exit.h"
#include "../utils/packet.h"
#include <stdlib.h>

void execute_exit(command *cmd, raw_array *packets, circular_list *history) {
    reset_cmd(cmd);
    reset_arr(packets, destroy_packet);
    destroy_list(history, destroy_buffer);
    exit(EXIT_SUCCESS);
}