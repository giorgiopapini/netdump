#ifndef PRINT_H
#define PRINT_H

#include "../command_handler.h"
#include "../utils/custom_dissectors.h"
#include "../utils/shared_lib.h"


void execute_print(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_dissectors);

#endif