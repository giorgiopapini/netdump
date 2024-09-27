#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct buffer {
    char *content;
    size_t len;
} buffer;

void populate(buffer *);
void destroy_buff(buffer *);

#endif