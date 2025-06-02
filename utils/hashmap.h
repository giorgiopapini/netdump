#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

typedef struct hashmap_entry {
    int key;
    void *value;
    struct hashmap_entry *next;
} hashmap_entry;

typedef struct hashmap {
    size_t buckets_count;  /* dimension of dinamically allocated array (buckets) */
    hashmap_entry **buckets;
} hashmap;

#define ADD_PROTO_HANDLER_ENTRY(map, protocol, layer, dissect_function, protocol_name) \
    do { \
        add_entry( \
            map, \
            create_hashmap_entry( \
                protocol, \
                create_protocol_handler(protocol, layer, dissect_function, protocol_name) \
            ) \
        ); \
    } \
    while (0)

hashmap_entry *create_hashmap_entry(int key, void *value);
hashmap *create_hashmap(size_t buckets_count);
unsigned long long fnv_hash_1a_64 (void *key, int len);
void add_entry(hashmap *map, hashmap_entry *new_entry);
hashmap_entry *get_entry(hashmap *map, int key);

/* if entry->value is not dinamically allocated pass NULL, otherwise pass "free" function */
void destroy_hashmap(hashmap *map, void (*free_value)(void *));

#endif