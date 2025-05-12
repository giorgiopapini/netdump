#ifndef SAVE_H
#define SAVE_H

#include "../utils/command.h"
#include "../utils/raw_array.h"


void print_cwd(void);
void execute_save(command *cmd, raw_array *packets);

#endif