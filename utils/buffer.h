#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include "circular_linked_list.h"

#define MAX_BUFFER_LEN 256

#define ARROW_UP_KEY 'A'
#define ARROW_DOWN_KEY 'B'
#define ARROW_RIGHT_KEY 'C'
#define ARROW_LEFT_KEY 'D'
#define CANC_KEY '3'
#define TILDE_KEY '~'
#define ENTER_KEY '\n'
#define BACKSPACE_KEY 127

typedef struct buffer {
    char content[MAX_BUFFER_LEN];
    size_t len;
    size_t cursor_pos;
    int status;
} buffer;

buffer *create_buffer(void);
void copy_buffer(buffer *src, buffer *dest);
buffer *copy_to_heap(buffer *src);
int compare_buffers(buffer *b1, buffer *b2);
void normalize_content(buffer *buff);
int populate(buffer *buff, circular_list *history);
int check_buffer_status(buffer *buff);
void clear_buffer(buffer *buff);
void destroy_buffer(void *buff);

#endif