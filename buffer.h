#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

#define MAX_BUFFER_LEN 256

typedef struct buffer {
    char content[MAX_BUFFER_LEN];
    size_t len;
    int status;
} buffer;

buffer *create_buffer();
void copy_buffer(buffer *src, buffer *dest);
buffer *copy_to_heap(buffer *src);
void populate(buffer *);
int check_buffer_status(buffer *);

#endif