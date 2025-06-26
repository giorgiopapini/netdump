#include "raw_array.h"

#include <stdlib.h>

#include "../status_handler.h"


raw_array *create_raw_array(int n) {
    raw_array *new_arr = malloc(sizeof *new_arr);
    CHECK_NULL_RET(new_arr, NULL);
    init_arr(new_arr, n);
    return new_arr;
}

void init_arr(raw_array *arr, int n) {
    if (0 > n) raise_error(NEGATIVE_ARR_LENGTH, 1, NULL, n);

    CHECK_NULL_EXIT(arr);
    arr->values = malloc((size_t)n * sizeof *(arr->values));  /* At this point n is certain to be > 0 */
    CHECK_NULL_EXIT(arr->values);

    arr->allocated = (size_t)n;
}

void allocate(raw_array *arr, int n) {
    void *new_ptr;

    CHECK_NULL_EXIT(arr);

    if (0 > n) raise_error(NEGATIVE_ARR_LENGTH, 1, NULL, n);
    if (0 == arr->allocated) init_arr(arr, n);
    else {        
        new_ptr = realloc(arr->values, (arr->allocated + (size_t)n) * sizeof(void *));  /* At this point n is certain to be > 0 */
        CHECK_NULL_EXIT(new_ptr);

        /* realloc copied everything to a different memory block, update arr-values to point to the freshly allocated block */
        if (new_ptr != arr->values) arr->values = new_ptr;
        CHECK_NULL_EXIT(arr->values);
        
        arr->allocated += (size_t)n;
    }
}

void insert(raw_array *arr, void *pkt) {
    CHECK_NULL_CONTINUE(arr);
    if ((arr->len + 1) > arr->allocated) allocate(arr, 1);
    /* allocating only 1 slot each time is suboptimal, should only happen (for now) when scanning infinite amount of packets */
    
    CHECK_NULL_EXIT(arr->values);
    arr->values[arr->len] = pkt;
    arr->len ++;
}

void * get(raw_array *arr, size_t n) {
    size_t index = (n - 1);
    if (NULL == arr) raise_error(NULL_POINTER, 0, NULL, VARNAME(arr), __FILE__);
    else if (NULL == arr->values) raise_error(NULL_POINTER, 0, UNSCANNED_NET_HINT, VARNAME(arr->values), __FILE__);
    else if (index >= arr->len) raise_error(INDEX_OUT_OF_BOUNDS, 0, NULL, n, arr->len);
    else return arr->values[index];
    
    return NULL;  /* if a previous condition is satisfied (an error occoured) than return NULL */
}

void reset_arr(raw_array *arr, void (*deallocate_content)(void *)) {
    size_t i;
    
    CHECK_NULL_EXIT(arr);

    for (i = 0; i < arr->len; i ++) deallocate_content(arr->values[i]);
    if (NULL != arr->values) free(arr->values);

    arr->allocated = 0;
    arr->len = 0;
    arr->values = NULL;
}