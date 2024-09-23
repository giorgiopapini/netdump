#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "status_handler.h"

void destroy_buff(buffer *buff) { free(buff->content); }

void populate(buffer *buff) {
    size_t bytes = getline(&buff->content, &buff->len, stdin);

    if (bytes <= 0) raise(INPUT_ERROR, 1);

    buff->content[bytes - 1] = 0;
    buff->len = bytes - 1;
}