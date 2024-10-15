#include <stdlib.h>
#include <stdio.h>

#include "raw_array.h"
#include "../status_handler.h"


void init_arr(raw_array *arr, int n) {
    if (0 > n) raise(NEGATIVE_N_PACKETS, 1, NULL, n);

    arr->values = (void **)malloc(n * sizeof(void *));
    if (NULL == arr->values) raise(NULL_POINTER, 1, NULL, "arr->values", __FILE__);

    arr->allocated = n;
}

void allocate(raw_array *arr, int n) {
    if (0 > n) raise(NEGATIVE_N_PACKETS, 1, NULL, n);
    
    if (0 == arr->allocated) init_arr(arr, n);
    else {        
        void *new_ptr = realloc(arr->values, (arr->allocated + n) * sizeof(void *));
        if (NULL == new_ptr) raise(NULL_POINTER, 1, NULL, "new_ptr", __FILE__);

        /* realloc copied everything to a different memory block, update arr-values to point to the freshly allocated block */
        if (new_ptr != arr->values) arr->values = new_ptr;
        if (NULL == arr->values) raise(NULL_POINTER, 1, NULL, "arr->values", __FILE__);
        
        arr->allocated += n;
    }
}

void insert(raw_array *arr, void *ptr_bytes) {
    if (NULL == arr) raise(NULL_POINTER, 0, NULL, "arr", __FILE__);
    if ((arr->len + 1) > arr->allocated) allocate(arr, 1);
    /* allocating only 1 slot each time is suboptimal, should only happen (for now) when scanning infinite amount of packets */
    
    arr->values[arr->len] = ptr_bytes;
    arr->len ++;
}

void * get_pkt(raw_array *arr, size_t index) {
    if (NULL == arr) raise(NULL_POINTER, 0, NULL, "arr", __FILE__);
    if (NULL == arr->values) raise(NULL_POINTER, 0, UNSCANNED_NET_HINT, "arr->values", __FILE__);
    if (index >= arr->len || index < 0) raise(INDEX_OUT_OF_BOUNDS, 0, NULL, index, arr->len);
    else return arr->values[index];
    
}

void reset_arr(raw_array *arr) {
    if (NULL == arr) raise(NULL_POINTER, 0, NULL, "arr", __FILE__);

    int i;
    for (i = 0; i < arr->len; i ++) free(arr->values[i]);
    free(arr->values);

    arr->allocated = 0;
    arr->len = 0;
    arr->values = NULL;
}