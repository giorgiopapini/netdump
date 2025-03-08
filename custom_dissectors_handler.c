#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include <string.h>

#include "custom_dissectors_handler.h"
#include "status_handler.h"


void populate_custom_dissectors(custom_dissectors *dissectors, protocol_handler_mapping **mappings) {
    int i;

    for (i = 0; mappings[i] != NULL; i ++) {
        dissector_add(
            mappings[i]->handler, 
            mappings[i]->proto_table_num, 
            dissectors
        );
    }
}

custom_dissectors *load_custom_dissectors(shared_libs *libs) {
    custom_dissectors *dissectors = create_custom_dissectors();
    protocol_handler_mapping **mappings;
    protocol_handler_mapping **(*get_custom_protocols_mapping)();
    char *error;
    int i;

    if (NULL == libs->handles) return NULL;

    for (i = 0; i < libs->count; i ++) {
        get_custom_protocols_mapping = dlsym(libs->handles[i], FUNCTION_NAME);

        error = dlerror();
        if (error != NULL) {
            raise_error(FUNCTION_NOT_FOUND_ERROR, 0, NULL, error);
            dlclose(libs->handles[i]);
            continue;
        }

        mappings = get_custom_protocols_mapping();
        populate_custom_dissectors(dissectors, mappings);
        
        destroy_mappings(mappings);
    }
    
    return dissectors;
}

