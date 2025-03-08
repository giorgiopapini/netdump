#include <stdlib.h>

#include "exit.h"
#include "../utils/packet.h"
#include "../custom_dissectors_handler.h"

void execute_exit(command *cmd, raw_array *packets, circular_list *history, shared_libs *libs, custom_dissectors *custom_dissectors) {
    reset_cmd(cmd);
    reset_arr(packets, destroy_packet);
    destroy_list(history, destroy_buffer);
    destroy_shared_libs(libs);
    destroy_custom_dissectors(custom_dissectors);
    exit(EXIT_SUCCESS);
}