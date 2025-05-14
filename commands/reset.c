#include "reset.h"

#include "../status_handler.h"
#include "../utils/packet.h"
#include "../utils/raw_array.h"


void execute_reset(raw_array *packets) {
    CHECK_NULL_EXIT(packets);

    if (0 == packets->allocated) print_success_msg(ARRAY_EMPTY_SUCCESS);
    else {
        reset_arr(packets, destroy_packet);
        print_success_msg(ARRAY_RESET_SUCCESS);
    }
}