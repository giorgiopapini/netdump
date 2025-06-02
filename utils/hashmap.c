#include "hashmap.h"

#include <stdlib.h>

#include "../status_handler.h"


hashmap_entry *create_hashmap_entry(int key, void *value) {
    hashmap_entry *new_entry = malloc(sizeof *new_entry);
    CHECK_NULL_EXIT(new_entry);

    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = NULL;
    return new_entry;
}

hashmap *create_hashmap(size_t buckets_count) {
    hashmap *new_hashmap = malloc(sizeof *new_hashmap);
    CHECK_NULL_EXIT(new_hashmap);

    new_hashmap->buckets = calloc(buckets_count, sizeof *(new_hashmap->buckets));
    CHECK_NULL_EXIT(new_hashmap->buckets);
    
    new_hashmap->buckets_count = buckets_count;
    return new_hashmap;
}

unsigned long long fnv_hash_1a_64 (void *key, int len) {
    /* void pointer: pointer to struct, len: sizeof(struct) */
    unsigned char *p = key;
    unsigned long long h = 0xcbf29ce484222325ULL;
    int i;

    for (i = 0; i < len; i ++) h = (h ^ p[i]) * 0x100000001b3ULL;
    return h;
}

void add_entry(hashmap *map, hashmap_entry *new_entry) {
    size_t index;

    CHECK_NULL_EXIT(map);
    CHECK_NULL_RET(new_entry);

    index = fnv_hash_1a_64(&new_entry->key, sizeof new_entry->key) % map->buckets_count;
    new_entry->next = map->buckets[index];
    map->buckets[index] = new_entry;
}

hashmap_entry *get_entry(hashmap *map, int key) {
    size_t index;
    hashmap_entry *res;

    CHECK_NULL_EXIT(map);

    index = fnv_hash_1a_64(&key, sizeof key) % map->buckets_count;
    if (index >= map->buckets_count) return NULL;

    res = map->buckets[index];
    while (NULL != res) {
        if (res->key == key) return res;
        res = res->next;
    }

    return NULL;
}

void destroy_hashmap(hashmap *map, void (*free_value)(void *)) {
    hashmap_entry *curr = NULL;
    hashmap_entry *next = NULL;
    size_t i;
    
    CHECK_NULL_RET(map);

    if (NULL == map->buckets) {
        free(map);
        return;
    }

    for (i = 0; i < map->buckets_count; i ++) {
        curr = map->buckets[i];
        while (curr) {
            next = curr->next;
            if (NULL != free_value) free_value(curr->value);
            free(curr);
            curr = next;
        }
    }

    free(map->buckets);
    free(map);
}