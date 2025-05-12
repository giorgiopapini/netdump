#ifndef DISSECTORS_H
#define DISSECTORS_H

#include "../utils/custom_dissectors.h"
#include "../utils/shared_lib.h"
#include "../utils/command.h"

#define LIB_ACTIVE      "active"
#define LIB_DEACTIVE    "deactive"


void change_dissector_status(int new_status, char *filenames, shared_libs *libs);
void add_and_load_dissectors(shared_libs *libs, custom_dissectors *dissectors, char *paths);
void delete_dissector(shared_libs *libs, custom_dissectors *dissectors, char *filenames);
void print_dissectors_list(shared_libs *libs);
void execute_dissectors(command *cmd, shared_libs *libs, custom_dissectors *custom_dissectors);

#endif