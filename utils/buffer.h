#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include "circular_linked_list.h"

#define MAX_BUFFER_LEN 256

typedef struct buffer {
    char content[MAX_BUFFER_LEN];
    size_t len;
    int status;
} buffer;

buffer *create_buffer();
void copy_buffer(buffer *src, buffer *dest);
buffer *copy_to_heap(buffer *src);
int compare_buffers(buffer *b1, buffer *b2);
int populate(buffer *buff, circular_list *history);
int check_buffer_status(buffer *buff);
void destroy_buffer(void *buff);

#endif