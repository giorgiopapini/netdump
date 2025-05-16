#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include "circular_linked_list.h"

#define MAX_BUFFER_LEN 256

#define _arrow_up_KEY 'A'
#define _arrow_down_KEY 'B'
#define _arrow_right_KEY 'C'
#define _arrow_left_KEY 'D'
#define _canc_KEY '3'
#define TILDE_KEY '~'
#define ENTER_KEY '\n'
#define _backspace_KEY 127

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