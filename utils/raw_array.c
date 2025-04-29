#include <stdlib.h>
#include <stdio.h>

#include "raw_array.h"
#include "../status_handler.h"


void init_arr(raw_array *arr, int n) {
    if (0 > n) raise_error(NEGATIVE_N_PACKETS, 1, NULL, n);

    arr->values = (void **)malloc(n * sizeof(void *));
    if (NULL == arr->values) raise_error(NULL_POINTER, 1, NULL, "arr->values", __FILE__);

    arr->allocated = n;
}

void allocate(raw_array *arr, int n) {
    void *new_ptr;

    if (0 > n) raise_error(NEGATIVE_N_PACKETS, 1, NULL, n);
    if (0 == arr->allocated) init_arr(arr, n);
    else {        
        new_ptr = realloc(arr->values, (arr->allocated + n) * sizeof(void *));
        if (NULL == new_ptr) raise_error(NULL_POINTER, 1, NULL, "new_ptr", __FILE__);

        /* realloc copied everything to a different memory block, update arr-values to point to the freshly allocated block */
        if (new_ptr != arr->values) arr->values = new_ptr;
        if (NULL == arr->values) raise_error(NULL_POINTER, 1, NULL, "arr->values", __FILE__);
        
        arr->allocated += n;
    }
}

void insert(raw_array *arr, void *pkt) {
    if (NULL == arr) raise_error(NULL_POINTER, 0, NULL, "arr", __FILE__);
    if ((arr->len + 1) > arr->allocated) allocate(arr, 1);
    /* allocating only 1 slot each time is suboptimal, should only happen (for now) when scanning infinite amount of packets */
    
    arr->values[arr->len] = pkt;
    arr->len ++;
}

void * get(raw_array *arr, size_t n) {
    size_t index = (n - 1);
    if (NULL == arr) raise_error(NULL_POINTER, 0, NULL, "arr", __FILE__);
    else if (NULL == arr->values) raise_error(NULL_POINTER, 0, UNSCANNED_NET_HINT, "arr->values", __FILE__);
    else if (index >= arr->len) raise_error(INDEX_OUT_OF_BOUNDS, 0, NULL, n, arr->len);
    else return arr->values[index];
    
    return NULL;  /* if a previous condition is satisfied (an error occoured) than return NULL */
}

void reset_arr(raw_array *arr, void (*deallocate_content)(void *)) {
    if (NULL == arr) raise_error(NULL_POINTER, 0, NULL, "arr", __FILE__);

    size_t i;
    for (i = 0; i < arr->len; i ++) deallocate_content(arr->values[i]);
    free(arr->values);

    arr->allocated = 0;
    arr->len = 0;
    arr->values = NULL;
}