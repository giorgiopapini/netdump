#include "buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../status_handler.h"
#include "string_utils.h"
#include "formats.h"

#define IS_LITERAL(c)                       (20 <= c && 126 >= c)

#define MOVE_CURSOR(x, y)                   (printf("\033[%d;%dH", (y), (x)))
#define MOVE_CURSOR_TO_END(pos, len)        do { if (pos != len) printf("\033[%ldC", len - pos); } while (0)
#define MOVE_CURSOR_TO_PREV_POS(pos, len)   do { if ((len - 1) != pos) printf("\033[%ldD", (len - 1) - pos); } while (0)
#define CLEAR_STRN(len)                     do { for (size_t z = (size_t)len; z > 0; z --) printf("\b \b"); } while (0)


static void _refresh_output(buffer *buff, size_t old_len);
static int _arrow_up(buffer *buff, circular_list *list, int *end);
static int _arrow_down(buffer *buff, circular_list *list, int *end);
static int _arrow_right(buffer *buff);
static int _arrow_left(buffer *buff);
static int _canc(buffer *buff);
static int _backspace(buffer *buff);
static int _literal_key(buffer *buff, int c);

buffer *create_buffer(void) {
    buffer *new_buff = (buffer *)malloc(sizeof(buffer));
    CHECK_NULL_EXIT(new_buff);

    memset(new_buff->content, 0, sizeof(new_buff->content));
    new_buff->len = 0;
    new_buff->status = 0;
    return new_buff;
}

void copy_buffer(buffer *src, buffer *dest) {  /* src and dest should not be NULL */
    size_t i;

    CHECK_NULL_EXIT(src);
    CHECK_NULL_EXIT(dest);

    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->len = src->len;
    dest->status = src->status;
}

buffer *copy_to_heap(buffer *src) {
    buffer *dest = create_buffer();
    size_t i;
    
    CHECK_NULL_RET(src, NULL);
    CHECK_NULL_RET(dest, NULL);

    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->len = src->len;
    dest->status = src->status;
    return dest;
}

int compare_buffers(buffer *b1, buffer *b2) {
    if (NULL == b1 && NULL == b2) return 1;
    /* if b1 and b2 are both NULL then return true, otherwise (xor between b1 and b2 being NULL) return false */
    
    CHECK_NULL_RET(b1, 0);
    CHECK_NULL_RET(b2, 0);
    
    if (b1->len != b2->len) return 0;
    if (b1->status != b2->status) return 0;

    /* checking even if content will never be NULL as it is defined in buffer */
    CHECK_NULL_EXIT(b1->content);
    CHECK_NULL_EXIT(b2->content);

    if ('\0' == b2->content[0] && '\0' == b1->content[0]) return 1;
    else if (0 != strcmp(b1->content, b2->content)) return 0;

    return 1;
}

void normalize_content(buffer *buff) {
    char *normalized_str;

    CHECK_NULL_EXIT(buff);
    normalized_str = get_trimmed_str(buff->content, buff->len);
    if (NULL == normalized_str) return;

    strncpy(buff->content, normalized_str, MAX_BUFFER_LEN - 1);
    free(normalized_str);
    
    buff->len = strlen(buff->content);
    lower_str_except_interval(buff->content, STRING_DELIMITER);
    buff->content[buff->len] = '\0';
}

static void _refresh_output(buffer *buff, size_t old_len) {   /* refresh string, the cursor is kept at the end of string */
    size_t i;

    CHECK_NULL_EXIT(buff);
    MOVE_CURSOR_TO_END(buff->cursor_pos, old_len);
    CLEAR_STRN(old_len);
    for (i = 0; i < buff->len; i ++) printf("%c", buff->content[i]);  /* print new string (prints also final '\0') */; 
}

static int _arrow_up(buffer *buff, circular_list *list, int *end) {
    size_t old_len;
    size_t prev_pos;

    CHECK_NULL_EXIT(buff);
    old_len = buff->len;
    prev_pos = buff->cursor_pos;

    CHECK_NULL_EXIT(list);
    if (NULL == list->head) return _arrow_up_KEY;

    CHECK_NULL_EXIT(list->curr);
    *end = 0;  /* not at the end of history */
    list->curr = list->curr->prev;
    memcpy(buff, list->curr->content, sizeof(buffer));
    buff->cursor_pos = prev_pos;
    _refresh_output(buff, old_len);  /* buff len is updated, but i need old length to make refresh work */
    buff->cursor_pos = buff->len;
    return _arrow_up_KEY;
}

static int _arrow_down(buffer *buff, circular_list *list, int *end) {
    size_t old_len;
    size_t prev_pos;

    CHECK_NULL_EXIT(buff);
    old_len = buff->len;
    prev_pos = buff->cursor_pos;

    CHECK_NULL_EXIT(list);
    if (NULL == list->head) return _arrow_down_KEY;
    if (list->curr == list->head) {
        CHECK_NULL_EXIT(end);
        if (*end) return _arrow_down_KEY;
        /* if at the end of history than nop. Otherwise execute arrow down behaviour */
    }

    CHECK_NULL_EXIT(list->curr);
    if (list->curr->next == list->head) {
        list->curr = list->curr->next;
        MOVE_CURSOR_TO_END(buff->cursor_pos, old_len);
        CLEAR_STRN(old_len);
        
        buff->len = 0;
        buff->cursor_pos = 0;

        CHECK_NULL_EXIT(end);
        *end = 1;  /* currently at the end of history */
        return _arrow_down_KEY;
    }

    list->curr = list->curr->next;
    memcpy(buff, list->curr->content, sizeof(buffer));
    buff->cursor_pos = prev_pos;
    _refresh_output(buff, old_len);  /* buff len is updated, but i need old length to make refresh work */
    buff->cursor_pos = buff->len;
    return _arrow_down_KEY;
}

static int _arrow_right(buffer *buff) {
    CHECK_NULL_EXIT(buff);

    if (buff->cursor_pos < buff->len && *(buff->content + buff->cursor_pos) != 0) {
        printf("\033[C");
        buff->cursor_pos ++;
    }
    return _arrow_right_KEY;
}

static int _arrow_left(buffer *buff) {
    CHECK_NULL_EXIT(buff);

    if (buff->cursor_pos > 0) {
        printf("\033[D");
        buff->cursor_pos --;
    }
    return _arrow_left_KEY;
}

static int _canc(buffer *buff) {
    size_t old_len;

    CHECK_NULL_EXIT(buff);
    old_len = buff->len;
    getch();
    if (buff->len == buff->cursor_pos) return _canc_KEY;

    delete_char(buff->content, buff->cursor_pos);
    buff->len --;
    _refresh_output(buff, old_len);
    MOVE_CURSOR_TO_PREV_POS(buff->cursor_pos, old_len);
    return _canc_KEY;
}

static int _backspace(buffer *buff) {
    size_t old_len;

    CHECK_NULL_EXIT(buff);
    old_len = buff->len;
    if (0 >= buff->cursor_pos) return _backspace_KEY;

    delete_char(buff->content, buff->cursor_pos - 1);
    buff->len --;
    _refresh_output(buff, old_len);

    if (buff->cursor_pos < old_len){
        MOVE_CURSOR_TO_PREV_POS(buff->cursor_pos, old_len);
        _arrow_left(buff);
    }
    else buff->cursor_pos --;
    return _backspace_KEY;
}

static int _literal_key(buffer *buff, int c) {
    size_t old_len;

    CHECK_NULL_EXIT(buff);
    old_len = buff->len;

    if (TILDE_KEY == c) return c;
    if (old_len >= MAX_BUFFER_LEN) {
        raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, MAX_BUFFER_LEN);
        return -1;
    }
    else {
        push_char(buff->content, MAX_BUFFER_LEN, buff->cursor_pos, c);
        buff->len ++;
        _refresh_output(buff, buff->len);
        MOVE_CURSOR_TO_PREV_POS(buff->cursor_pos, buff->len);
        buff->cursor_pos ++;
    }
    return c;
}

int populate(buffer *buff, circular_list *history) {
    int c;
    int end = 1;
    int ret = -1;

    CHECK_NULL_EXIT(buff);
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
    else if (127 == c || 8 == c) {  /* if pressed key == _backspace */
        if (buff->len > 0) ret = _backspace(buff);
    }
    else if ('\033' == c) {  /* catch special keys, add behaviour only to arrow keys (for now) */
        c = getch();
        switch(c = getch()) {
            case _arrow_up_KEY:          ret = _arrow_up(buff, history, &end); break;
            case _arrow_down_KEY:        ret = _arrow_down(buff, history, &end); break;
            case _arrow_right_KEY:       ret = _arrow_right(buff); break;
            case _arrow_left_KEY:        ret = _arrow_left(buff); break;
            case _canc_KEY:              ret = _canc(buff); break;
            default:                    ret = -1; break;
        }
    }
    else if (IS_LITERAL(c)) _literal_key(buff, c);

    fflush(stdout);
    return ret;
}

int check_buffer_status(buffer *buff) {
    CHECK_NULL_EXIT(buff);
    switch (buff->status) {
        case BUFFER_OVERFLOW_ERROR:     printf("\n"); raise_error(BUFFER_OVERFLOW_ERROR, 1, NULL, __FILE__, MAX_BUFFER_LEN); break;
        default: break;
    }
    return buff->status;
}

void clear_buffer(buffer *buff) {
    CHECK_NULL_RET(buff);
    buff->content[0] = '\0';
    buff->cursor_pos = 0;
    buff->len = 0;
    buff->status = 0;
}

void destroy_buffer(void *buff) { /* it could be more complex in a future version */
    CHECK_NULL_EXIT(buff);
    if (NULL != buff) free(buff);
}