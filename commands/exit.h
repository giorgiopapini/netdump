#ifndef EXIT_H
#define EXIT_H

#include "../command_handler.h"
#include "../utils/circular_linked_list.h"

void execute_exit(command *cmd, raw_array *packets, circular_list *history);

#endif