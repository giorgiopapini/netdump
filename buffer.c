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

#define MOVE_CURSOR(x, y)                   (printf("\033[%d;%dH", (y), (x)))
#define MOVE_CURSOR_TO_END(pos, len)        do { if (pos != len) printf("\033[%dC", len - pos); } while (0)
#define MOVE_CURSOR_TO_PREV_POS(pos, len)   do { if ((len - 1) != pos) printf("\033[%dD", (len - 1) - pos); } while (0)
#define CLEAR_STRN(len)                     do { for (int i = len; i > 0; i --) printf("\b \b"); } while (0)

buffer *create_buffer() {
    buffer *new_buff = (buffer *)malloc(sizeof(buffer));
    if (NULL == new_buff) raise_error(NULL_POINTER, 1, NULL, "new_buff", __FILE__);
    
    memset(new_buff->content, 0, sizeof(new_buff->content));
    new_buff->len = 0;
    new_buff->status = 0;
    return new_buff;
}

void copy_buffer(buffer *src, buffer *dest) {
    int i;
    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->len = src->len;
    dest->status = src->status;
}

buffer *copy_to_heap(buffer *src) {
    buffer *dest = create_buffer();
    int i;
    
    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->len = src->len;
    dest->status = src->status;
    return dest;
}

int compare_buffers(buffer *b1, buffer *b2) {
    if (NULL == b1 || NULL == b2) raise_error(NULL_POINTER, 1, NULL, "b1 || b2", __FILE__);
    
    if (b1->len != b2->len) return 0;
    if (b1->status != b2->status) return 0;
    
    if (NULL == b2->content && NULL == b1->content) return 1;
    else if (0 != strcmp(b1->content, b2->content)) return 0;

    return 1;
}

void normalize_content(buffer *buff) {
    char *normalized_str = get_trimmed_str(buff->content);
    strncpy(buff->content, normalized_str, MAX_BUFFER_LEN - 1);
    buff->content[buff->len] = '\0';
}

void refresh_output(buffer *buff, int pos, int old_len) {   /* refresh string, the cursor is kept at the end of string */
    int i;
    
    MOVE_CURSOR_TO_END(pos, old_len);
    CLEAR_STRN(old_len);   
    for (i = 0; i < buff->len; i ++) printf("%c", buff->content[i]);  /* print new string (prints also final '\0') */; 
}

void arrow_up(buffer *buff, int *pos, circular_list *list, int *end) {
    int old_len = buff->len;
    if (NULL == list->head) return;

    *end = 0;  /* not at the end of history */
    list->curr = list->curr->prev;
    memcpy(buff, list->curr->content, sizeof(buffer));
    refresh_output(buff, *pos, old_len);  /* buff len is updated, but i need old length to make refresh work */
    *pos = buff->len;
}

void arrow_down(buffer *buff, int *pos, circular_list *list, int *end) {
    int old_len = buff->len;
    int i;

    if (NULL == list->head) return;
    if (list->curr == list->head) {
        if (*end) return;  /* if at the end of history than nop. Otherwise execute arrow down behaviour */
    }
    if (list->curr->next == list->head) {
        list->curr = list->curr->next;
        MOVE_CURSOR_TO_END(*pos, old_len);
        CLEAR_STRN(old_len);
        
        buff->len = 0;
        *pos = 0;
        *end = 1;  /* currently at the end of history */
        return;
    }

    list->curr = list->curr->next;
    memcpy(buff, list->curr->content, sizeof(buffer));
    refresh_output(buff, *pos, old_len);  /* buff len is updated, but i need old length to make refresh work */
    *pos = buff->len;
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
    int old_len = buff->len;
    int i;
    getch();
    if (buff->len == *pos) return;

    delete_char(buff->content, *pos);
    buff->len --;
    refresh_output(buff, *pos, old_len);
    MOVE_CURSOR_TO_PREV_POS(*pos, old_len);
}

void backspace(buffer *buff, int *pos) {
    int old_len = buff->len;
    if (0 >= *pos) return;

    delete_char(buff->content, *pos - 1);
    buff->len --;
    refresh_output(buff, *pos, old_len);
    MOVE_CURSOR_TO_PREV_POS(*pos, old_len);

    if (*pos < old_len) arrow_left(buff, pos);
    else *pos = *pos - 1;
}

void literal_key(buffer *buff, int *pos, char c) {
    int old_len = buff->len;
    if ((old_len + 1) > MAX_BUFFER_LEN) {
        raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, MAX_BUFFER_LEN);
        return;
    }
    else {
        push_char(buff->content, MAX_BUFFER_LEN, *pos, c);
        buff->len ++;
        refresh_output(buff, *pos, buff->len);
        MOVE_CURSOR_TO_PREV_POS(*pos, buff->len);
        *pos = *pos + 1;
    }
}

void populate(buffer *buff, circular_list *history) {
    char c = 0;
    int pos = 0;
    int end = 1;

    memset(buff->content, 0, buff->len);
    buff->len = 0;
    buff->status = 0;

    if (NULL != history->curr) history->curr = history->head;  /* at each populate() call reset history->curr position */

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
                case ARROW_UP_KEY:          arrow_up(buff, &pos, history, &end); break;
                case ARROW_DOWN_KEY:        arrow_down(buff, &pos, history, &end); break;
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

void destroy_buffer(void *buff) { /* it could be more complex in a future version */
    if (NULL == buff) raise_error(NULL_POINTER, 1, NULL, "buff", __FILE__);
    else free(buff);
}
