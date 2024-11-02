#ifndef RAW_ARRAY_H
#define RAW_ARRAY_H

#include <stddef.h>

#define LEN(type, ...)                (sizeof((type[]){__VA_ARGS__}) / sizeof(type))


typedef struct raw_array {
    size_t allocated;
    size_t len;
    void **values;
} raw_array;


void init_arr(raw_array *arr, int n);
void allocate(raw_array *arr, int n);   /* n = number of slot to allocate */
void insert(raw_array *arr, void *ptr_bytes);
void * get(raw_array *arr, size_t index);
void reset_arr(raw_array *arr);

#endif