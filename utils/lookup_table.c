#include <stdio.h>  /* maybe not needed in the future */
#include "lookup_table.h"


const char * get_value(entry *table, uint16_t target_key, size_t len) {
    int i;
    
    for (i = 0; i < len; i ++) {
        if (table[i].key == target_key) return table[i].value;
    }
    return NULL;
}