#ifndef PRINT_H
#define PRINT_H

#include "../utils/shared_lib.h"
#include "../utils/command.h"
#include "../utils/raw_array.h"
#include "../utils/custom_dissectors.h"


void execute_print(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_diss);

#endif