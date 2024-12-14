#include <stdio.h>  /* maybe not needed in the future */
#include "lookup_table.h"


const char * get_value(entry *table, uint16_t target_key) {
    int i;

    for (i = 0; ;i ++) {
        if (table[i].key == 0 && table[i].value == NULL) return NULL;
        if (table[i].key == target_key) return table[i].value;
    }
    return NULL;
}