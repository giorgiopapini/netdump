#ifndef PRINT_H
#define PRINT_H

#include "../command_handler.h"
#include "../utils/custom_dissectors.h"


void execute_print(command *cmd, raw_array *packets, custom_dissectors *custom_dissectors);

#endif