#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "buffer.h"
#include "status_handler.h"
#include "utils/string_utils.h"


#define ARROW_UP_KEY 'A'
#define ARROW_DOWN_KEY 'B'
#define ARROW_RIGHT_KEY 'C'
#define ARROW_LEFT_KEY 'D'
#define CANC_KEY '3'


void normalize_content(buffer *buff) {
    char *normalized_str = get_trimmed_str(buff->content);
    strncpy(buff->content, normalized_str, MAX_BUFFER_LEN - 1);
    buff->content[buff->len] = '\0';
}

void refresh_output(buffer *buff, int pos) {
    int i;

    if (pos != buff->len) printf("\033[%dC", buff->len - pos);  /* move the cursor to end of string if not alredy there */
    for (i = buff->len; i > 0; i --) printf("\b \b");  /* delete old string */   
    for (i = 0; i < buff->len; i ++) printf("%c", buff->content[i]);  /* print updated string (prints also final '\0') */;  

    if ((buff->len - 1) != pos) printf("\033[%dD", (buff->len - 1) - pos);  /* reset the pointer to previous position */
    /* refresh_output() should not change any state of the buffer. The length is then decremented in the caller function */
}

void arrow_up() {
    printf("|up|");
}

void arrow_down() {
    printf("|down|");
}

void arrow_right(buffer *buff, int *pos) {
    if (*pos < buff->len || *(buff->content + *pos) != 0) {
        printf("\033[C");
        *pos = *pos + 1;
    }
}

void arrow_left(buffer *buff, int *pos) {
    if (*pos > 0) {
        printf("\033[D");
        *pos = *pos - 1;
    }
}

void canc(buffer *buff, int *pos) {
    int i;
    getch();
    if (buff->len == *pos) return;

    delete_char(buff->content, *pos);
    refresh_output(buff, *pos);
    buff->len --;
}

void backspace(buffer *buff, int *pos) {
    if (0 >= *pos) return;

    delete_char(buff->content, *pos - 1);
    refresh_output(buff, *pos);

    if (*pos < buff->len) arrow_left(buff, pos);
    else *pos = *pos - 1;
    buff->len --;
}

void literal_key(buffer *buff, int *pos, char c) {
    if ((buff->len + 1) > MAX_BUFFER_LEN) {
        raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, MAX_BUFFER_LEN);
        return;
    }
    else {
        push_char(buff->content, MAX_BUFFER_LEN, *pos, c);
        buff->len ++;
        refresh_output(buff, *pos);
        *pos = *pos + 1;
    }
}

void populate(buffer *buff) {
    char c = 0;
    int pos = 0;

    memset(buff->content, 0, buff->len);
    buff->len = 0;
    buff->status = 0;

    while (c != '\n') {
        if (buff->len + 1 >= MAX_BUFFER_LEN) {
            buff->status = BUFFER_OVERFLOW_ERROR;
            break;
        }

        c = getch();
        if (c == 127 || c == 8) {   /* if pressed key == backspace */
            if (buff->len > 0) backspace(buff, &pos);
        }
        else if (c == '\033') {  /* catch special keys, add behaviour only to arrow keys (for now) */
            getch();
            switch(getch()) {
                case ARROW_UP_KEY:          arrow_up(); break;
                case ARROW_DOWN_KEY:        arrow_down(); break;
                case ARROW_RIGHT_KEY:       arrow_right(buff, &pos); break;
                case ARROW_LEFT_KEY:        arrow_left(buff, &pos); break;
                case CANC_KEY:              canc(buff, &pos); break;
                default:                    printf("\n"); break;    /* (e.g.) absorbing the esc char */
            }
        }
        else if (c != '\n') literal_key(buff, &pos, c);
    }
    printf("\n");   /* outside the while loop, it means that the enter key has been pressed */

    buff->content[buff->len] = '\0';
    normalize_content(buff);
}

int check_buffer_status(buffer *buff) {
    switch (buff->status) {
        case BUFFER_OVERFLOW_ERROR:     printf("\n"); raise_error(BUFFER_OVERFLOW_ERROR, 1, NULL, __FILE__, MAX_BUFFER_LEN); break;
        default: break;
    }
    return buff->status;
}
