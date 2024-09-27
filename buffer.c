#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "status_handler.h"
#include "utils/string_utils.h"

void destroy_buff(buffer *buff) { free(buff->content); }

void normalize_content(buffer *buff) {
    char *normalized_str = get_trimmed_str(buff->content);
    free(buff->content);
    buff->content = normalized_str;
}

void populate(buffer *buff) {
    size_t bytes = getline(&buff->content, &buff->len, stdin);

    if (bytes <= 0) raise(INPUT_ERROR, 1);

    buff->content[bytes - 1] = 0;
    buff->len = bytes - 1;
    normalize_content(buff);
}