#ifndef EXIT_H
#define EXIT_H

#include "../command_handler.h"
#include "../utils/shared_lib.h"
#include "../utils/circular_linked_list.h"
#include "../utils/custom_dissectors.h"

void execute_exit(command *cmd, raw_array *packets, circular_list *history, shared_libs *libs, custom_dissectors *custom_dissectors);

#endif