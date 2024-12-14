#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H

#include <stddef.h>
#include <stdint.h>

typedef struct entry {
    uint16_t key;       /* find the optimal type for the key value */
    const char *value;
} entry;

typedef struct entry lookup_table[];

const char * get_value(entry *table, uint16_t target_key);

#endif