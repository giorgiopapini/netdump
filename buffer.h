#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

#define MAX_BUFFER_LEN 256

typedef struct buffer {
    char content[MAX_BUFFER_LEN];
    size_t len;
    int status;
} buffer;

void populate(buffer *);
int check_buffer_status(buffer *);

#endif