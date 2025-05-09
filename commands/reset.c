#include "reset.h"
#include "../status_handler.h"
#include "../utils/packet.h"

void execute_reset(raw_array *packets) {
    if (0 == packets->allocated) print_success_msg(ARRAY_EMPTY_SUCCESS);
    else {
        reset_arr(packets, destroy_packet);
        print_success_msg(ARRAY_RESET_SUCCESS);
    }
}