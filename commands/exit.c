#include "exit.h"
#include <stdlib.h>

void execute_exit(command *cmd, raw_array *packets) {
    reset_cmd(cmd);
    reset_arr(packets);
    exit(EXIT_SUCCESS);
}