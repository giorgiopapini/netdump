#include "reset.h"
#include "../status_handler.h"

void execute_reset(command *cmd, raw_array *packets) {
    if (0 == packets->allocated) print_success_msg(ARRAY_EMPTY_SUCCESS);
    else {
        reset_arr(packets);
        print_success_msg(ARRAY_RESET_SUCCESS);
    }
}