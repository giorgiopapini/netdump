#include "custom_dissectors_handler.h"


custom_dissectors *load_custom_dissectors(shared_libs *libs) {
    custom_dissectors *dissectors = create_custom_dissectors();
    int i;

    if (NULL != libs->handles) {
        for (i = 0; i < libs->count; i ++) load_dissector(dissectors, libs->handles[i], libs->filenames[i]);
    }
    
    return dissectors;
}

