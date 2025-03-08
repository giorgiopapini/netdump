#ifndef CUSTOM_DISSECTORS_HANDLER_H
#define CUSTOM_DISSECTORS_HANDLER_H

#include "utils/custom_dissectors.h"
#include "utils/shared_lib.h"

#define FUNCTION_NAME       "get_custom_protocols_mapping"


custom_dissectors *load_custom_dissectors(shared_libs *libs);

#endif