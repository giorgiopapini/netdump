#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "buffer.h"
#include "../status_handler.h"
#include "../command_handler.h"
#include "string_utils.h"
#include "formats.h"

#define IS_LITERAL(c)                       (20 <= c && 126 >= c)

#define MOVE_CURSOR(x, y)                   (printf("\033[%d;%dH", (y), (x)))
#define MOVE_CURSOR_TO_END(pos, len)        do { if (pos != len) printf("\033[%ldC", len - pos); } while (0)
#define MOVE_CURSOR_TO_PREV_POS(pos, len)   do { if ((len - 1) != pos) printf("\033[%ldD", (len - 1) - pos); } while (0)
#define CLEAR_STRN(len)                     do { for (size_t z = (size_t)len; z > 0; z --) printf("\b \b"); } while (0)


void refresh_output(buffer *buff, size_t old_len);
int arrow_up(buffer *buff, circular_list *list, int *end);
int arrow_down(buffer *buff, circular_list *list, int *end);
int arrow_right(buffer *buff);
int arrow_left(buffer *buff);
int canc(buffer *buff);
int backspace(buffer *buff);
int literal_key(buffer *buff, int c);

buffer *create_buffer(void) {
    buffer *new_buff = (buffer *)malloc(sizeof(buffer));
    if (NULL == new_buff) raise_error(NULL_POINTER, 1, NULL, "new_buff", __FILE__);
    
    memset(new_buff->content, 0, sizeof(new_buff->content));
    new_buff->len = 0;
    new_buff->status = 0;
    return new_buff;
}

void copy_buffer(buffer *src, buffer *dest) {
    size_t i;
    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->len = src->len;
    dest->status = src->status;
}

buffer *copy_to_heap(buffer *src) {
    buffer *dest = create_buffer();
    size_t i;
    
    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->len = src->len;
    dest->status = src->status;
    return dest;
}

int compare_buffers(buffer *b1, buffer *b2) {
    if (NULL == b1 || NULL == b2) raise_error(NULL_POINTER, 1, NULL, "b1 || b2", __FILE__);
    
    if (b1->len != b2->len) return 0;
    if (b1->status != b2->status) return 0;

    if ('\0' == b2->content[0] && '\0' == b1->content[0]) return 1;
    else if (0 != strcmp(b1->content, b2->content)) return 0;

    return 1;
}

void normalize_content(buffer *buff) {
    char *normalized_str = get_trimmed_str(buff->content, buff->len);
    if (NULL == normalized_str) return;
    
    strncpy(buff->content, normalized_str, MAX_BUFFER_LEN - 1);
    free(normalized_str);
    
    buff->len = strlen(buff->content);
    lower_str_except_interval(buff->content, STRING_DELIMITER);
    buff->content[buff->len] = '\0';
}

void refresh_output(buffer *buff, size_t old_len) {   /* refresh string, the cursor is kept at the end of string */
    size_t i;
    
    MOVE_CURSOR_TO_END(buff->cursor_pos, old_len);
    CLEAR_STRN(old_len);
    for (i = 0; i < buff->len; i ++) printf("%c", buff->content[i]);  /* print new string (prints also final '\0') */; 
}

int arrow_up(buffer *buff, circular_list *list, int *end) {
    size_t old_len = buff->len;
    size_t prev_pos = buff->cursor_pos;
    if (NULL == list->head) return ARROW_UP_KEY;

    *end = 0;  /* not at the end of history */
    list->curr = list->curr->prev;
    memcpy(buff, list->curr->content, sizeof(buffer));
    buff->cursor_pos = prev_pos;
    refresh_output(buff, old_len);  /* buff len is updated, but i need old length to make refresh work */
    buff->cursor_pos = buff->len;
    return ARROW_UP_KEY;
}

int arrow_down(buffer *buff, circular_list *list, int *end) {
    size_t old_len = buff->len;
    size_t prev_pos = buff->cursor_pos;

    if (NULL == list->head) return ARROW_DOWN_KEY;
    if (list->curr == list->head) {
        if (*end) return ARROW_DOWN_KEY;  /* if at the end of history than nop. Otherwise execute arrow down behaviour */
    }
    if (list->curr->next == list->head) {
        list->curr = list->curr->next;
        MOVE_CURSOR_TO_END(buff->cursor_pos, old_len);
        CLEAR_STRN(old_len);
        
        buff->len = 0;
        buff->cursor_pos = 0;
        *end = 1;  /* currently at the end of history */
        return ARROW_DOWN_KEY;
    }

    list->curr = list->curr->next;
    memcpy(buff, list->curr->content, sizeof(buffer));
    buff->cursor_pos = prev_pos;
    refresh_output(buff, old_len);  /* buff len is updated, but i need old length to make refresh work */
    buff->cursor_pos = buff->len;
    return ARROW_DOWN_KEY;
}

int arrow_right(buffer *buff) {
    if (buff->cursor_pos < buff->len || *(buff->content + buff->cursor_pos) != 0) {
        printf("\033[C");
        buff->cursor_pos ++;
    }
    return ARROW_RIGHT_KEY;
}

int arrow_left(buffer *buff) {
    if (buff->cursor_pos > 0) {
        printf("\033[D");
        buff->cursor_pos --;
    }
    return ARROW_LEFT_KEY;
}

int canc(buffer *buff) {
    size_t old_len = buff->len;
    getch();
    if (buff->len == buff->cursor_pos) return CANC_KEY;

    delete_char(buff->content, buff->cursor_pos);
    buff->len --;
    refresh_output(buff, old_len);
    MOVE_CURSOR_TO_PREV_POS(buff->cursor_pos, old_len);
    return CANC_KEY;
}

int backspace(buffer *buff) {
    size_t old_len = buff->len;
    if (0 >= buff->cursor_pos) return BACKSPACE_KEY;

    delete_char(buff->content, buff->cursor_pos - 1);
    buff->len --;
    refresh_output(buff, old_len);

    if (buff->cursor_pos < old_len){
        MOVE_CURSOR_TO_PREV_POS(buff->cursor_pos, old_len);
        arrow_left(buff);
    }
    else buff->cursor_pos --;
    return BACKSPACE_KEY;
}

int literal_key(buffer *buff, int c) {
    size_t old_len = buff->len;

    if (TILDE_KEY == c) return c;

    if (old_len >= MAX_BUFFER_LEN) {
        raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, MAX_BUFFER_LEN);
        return -1;
    }
    else {
        push_char(buff->content, MAX_BUFFER_LEN, buff->cursor_pos, c);
        buff->len ++;
        refresh_output(buff, buff->len);
        MOVE_CURSOR_TO_PREV_POS(buff->cursor_pos, buff->len);
        buff->cursor_pos ++;
    }
    return c;
}

int populate(buffer *buff, circular_list *history) {
    int c;
    int end = 1;
    int ret = -1;

    if (buff->len >= MAX_BUFFER_LEN) {
        buff->status = BUFFER_OVERFLOW_ERROR;
        return -1;
    }
    
    c = getch();
    if (ENTER_KEY == c) {
        buff->content[buff->len] = '\0';
        normalize_content(buff);
        ret = ENTER_KEY;
        printf("\n");
    }
    else if (127 == c || 8 == c) {  /* if pressed key == backspace */
        if (buff->len > 0) ret = backspace(buff);
    }
    else if ('\033' == c) {  /* catch special keys, add behaviour only to arrow keys (for now) */
        c = getch();
        switch(c = getch()) {
            case ARROW_UP_KEY:          ret = arrow_up(buff, history, &end); break;
            case ARROW_DOWN_KEY:        ret = arrow_down(buff, history, &end); break;
            case ARROW_RIGHT_KEY:       ret = arrow_right(buff); break;
            case ARROW_LEFT_KEY:        ret = arrow_left(buff); break;
            case CANC_KEY:              ret = canc(buff); break;
            default:                    ret = -1; break;
        }
    }
    else if (IS_LITERAL(c)) literal_key(buff, c);

    fflush(stdout);
    return ret;
}

int check_buffer_status(buffer *buff) {
    switch (buff->status) {
        case BUFFER_OVERFLOW_ERROR:     printf("\n"); raise_error(BUFFER_OVERFLOW_ERROR, 1, NULL, __FILE__, MAX_BUFFER_LEN); break;
        default: break;
    }
    return buff->status;
}

void clear_buffer(buffer *buff) {
    buff->content[0] = '\0';
    buff->cursor_pos = 0;
    buff->len = 0;
    buff->status = 0;
}

void destroy_buffer(void *buff) { /* it could be more complex in a future version */
    if (NULL == buff) raise_error(NULL_POINTER, 1, NULL, "buff", __FILE__);
    else free(buff);
}