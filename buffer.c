#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "buffer.h"
#include "status_handler.h"
#include "utils/string_utils.h"


void normalize_content(buffer *buff) {
    char *normalized_str = get_trimmed_str(buff->content);
    strncpy(buff->content, normalized_str, MAX_BUFFER_LEN - 1);
    buff->content[buff->len] = '\0';
}

void populate(buffer *buff) {
    char c = 0;
    buff->content[0] = '\0';
    buff->len = 0;
    buff->status = 0;

    while (c != '\n') {
        if (buff->len + 1 >= MAX_BUFFER_LEN) {
            buff->status = BUFFER_OVERFLOW_ERROR;
            break;
        }

        c = getch();
        if (c == 127 || c == 8) {   /* if pressed key == backspace */
            if (buff->len > 0) {
                printf("\b \b");    /* erase previous printf output char */
                buff->content[buff->len] = ' ';
                buff->len --;
            }
        }
        else if (c != '\n') {
            buff->content[buff->len] = c;
            buff->len ++;
        };
        printf("%c", c);
    }

    buff->content[buff->len] = '\0';
    normalize_content(buff);
}

int check_buffer_status(buffer *buff) {
    switch (buff->status) {
        case BUFFER_OVERFLOW_ERROR:     printf("\n"); raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, MAX_BUFFER_LEN); break;
        default: break;
    }
    return buff->status;
}
