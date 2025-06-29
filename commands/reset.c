#include "reset.h"

#include "../status_handler.h"
#include "../utils/packet.h"
#include "../utils/raw_array.h"
#include "../utils/hierarchy.h"


void execute_reset(raw_array *packets, hierarchy_node *root) {
    CHECK_NULL_EXIT(packets);
    CHECK_NULL_EXIT(root);

    if (0 == packets->allocated) print_success_msg(ARRAY_EMPTY_SUCCESS, 1);
    else {
        reset_arr(packets, destroy_packet);
        print_success_msg(ARRAY_RESET_SUCCESS, 1);
    }

    reset_hierarchy(root);
}