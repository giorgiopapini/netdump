#ifndef DISSECTORS_H
#define DISSECTORS_H

#include "../command_handler.h"
#include "../utils/custom_dissectors.h"
#include "../utils/shared_lib.h"

#define LIB_ACTIVE      "active"
#define LIB_DEACTIVE    "deactive"


void execute_dissectors(command *cmd, shared_libs *libs, custom_dissectors *custom_dissectors);

#endif