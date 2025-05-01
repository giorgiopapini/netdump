#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <math.h>
#include <string.h>

#include "visualizer.h"
#include "../status_handler.h"
#include "colors.h"

#define STARTING_X_VAL 1
#define FIELD_MIN_Y_LEN 3
#define MAX_X 80

#define MOVE_CURSOR(x, y)                       (printf("\033[%ld;%ldH", (long)(y), (long)(x)))
#define MAX(x, y)                               ((x > y) ? x : y)
#define MIN(x, y)                               ((x > y) ? y : x)
#define Y_DEVIATION(rows)                       (rows + 1)  /* considering field rows + bottom border row */
#define PRINTABLE_CHARS(len, cols, max_x)       (MIN(MIN(cols, max_x), len))
#define OFFSET_LEFT(max, min)                   ((max - min) / 2)
#define OFFSET_RIGHT(max, min) \
    ((size_t)(((max % min) == 0) ? round((double)(max - min) / 2.0) : round((double)(max - min) / 2.0)))


void get_terminal_size(size_t *cols, size_t *rows);
void print_horizontal_border(size_t len, size_t *curr_x, size_t *curr_y);
void print_line(const char *val, size_t *curr_x, size_t *curr_y, size_t offset_left, size_t offset_right);
void print_value(const char *label, const char *content, size_t *curr_x, size_t *curr_y, size_t max_len);

size_t prev_used_rows = 0;

void start_printing() { prev_used_rows = 0; }

void end_printing() {
    move_to_next_line(NULL, NULL, prev_used_rows);
    prev_used_rows = 0;
}

void get_terminal_size(size_t *cols, size_t *rows) {
    struct winsize w;
    if (-1 == ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) raise_error(TERMINAL_SIZE_ERROR, 1, NULL);

    *cols = w.ws_col;
    *rows = w.ws_row;
}

size_t calc_rows(const char *str) {
    size_t cols, rows;
    size_t usable_cols;
    size_t max_len;
    
    if (!str) raise_error(NULL_POINTER, 1, NULL);

    max_len = strlen(str) + (2 * strlen(MARGIN)) + (2 * strlen(VERTICAL_BORDER));
    get_terminal_size(&cols, &rows);

    usable_cols = MIN(cols, MAX_X);
    if (usable_cols == 0) raise_error(TERMINAL_SIZE_ERROR, 1, NULL);

    return (max_len + usable_cols - 1) / usable_cols;
}

int get_cursor_position(size_t *col, size_t *row) {
    char buf[32];
    unsigned int i = 0;
    ssize_t ret;

    struct termios saved, raw;
    tcgetattr(STDIN_FILENO, &saved);

    raw = saved;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    printf("\033[6n");
    fflush(stdout);

    while (i < sizeof(buf) - 1) {
        ret = read(STDIN_FILENO, buf + i, 1);
        if (ret != 1 || buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &saved);

    if (buf[0] == '\033' && buf[1] == '[') {
        sscanf(buf + 2, "%ld;%ld", row, col);
    }
    else return -1;
    return 0;
}

void move_to_next_line(size_t *curr_x, size_t *curr_y, size_t used_rows) {
    if (NULL == curr_x && NULL == curr_y) {
        size_t x, y;
        get_cursor_position(&x, &y);
        MOVE_CURSOR(0, y + Y_DEVIATION(used_rows));
        printf("\n");
    }
    else {
        *curr_x = STARTING_X_VAL;
        *curr_y += Y_DEVIATION(used_rows);
        MOVE_CURSOR(*curr_x, *curr_y);
    }
}

void print_horizontal_border(size_t len, size_t *curr_x, size_t *curr_y) {
    size_t i;
    len -= 2 * strlen(JUNCTION);  /* this exclude space needed for the junctions inside the for loop */

    MOVE_CURSOR(*curr_x, *curr_y);
    printf(CONT_JUNCTION);
    for (i = 0; i < len; i ++) printf(CONT_HORIZONTAL_BORDER);
    printf(CONT_JUNCTION);
}

void print_line(const char *val, size_t *curr_x, size_t *curr_y, size_t offset_left, size_t offset_right) {
    printf("\n");  /* creates new line if no more rows available in terminal */
    
    *curr_y += 1; 
    MOVE_CURSOR(*curr_x, *curr_y);
    
    printf(CONT_VERTICAL_BORDER MARGIN);
    for (; offset_left > 0; offset_left --) printf(" ");
    printf("%s", val);
    for (; offset_right > 0; offset_right --) printf(" ");
    printf(MARGIN CONT_VERTICAL_BORDER);
}

void print_value(const char *label, const char *content, size_t *curr_x, size_t *curr_y, size_t max_len) {
    size_t label_len = strlen(label);
    size_t content_len = strlen(content);
    size_t used_rows;
    char partial_str[MAX_X];
    size_t partial_i;
    size_t i;

    partial_i = (max_len - (2 * strlen(MARGIN)) - (2 * strlen(VERTICAL_BORDER)));

    if (label_len > content_len) {
        used_rows = calc_rows(label);
        if (1 == used_rows) {
            print_line(label, curr_x, curr_y, 0, 0);
            print_line(content, curr_x, curr_y, OFFSET_LEFT(label_len, content_len), OFFSET_RIGHT(label_len, content_len));
        }
        else {
            for (i = 1; i <= used_rows; i ++) {
                strncpy(partial_str, label, partial_i);
                partial_str[partial_i] = '\0';

                print_line(partial_str, curr_x, curr_y, OFFSET_LEFT(partial_i, strlen(partial_str)), OFFSET_RIGHT(partial_i, strlen(partial_str)));
                label += partial_i;
            }
            print_line(content, curr_x, curr_y, OFFSET_LEFT(partial_i, content_len), OFFSET_RIGHT(partial_i, content_len));
        }
    }
    else {
        used_rows = calc_rows(content);
        if (1 == used_rows) {
            print_line(label, curr_x, curr_y, OFFSET_LEFT(content_len, label_len), OFFSET_RIGHT(content_len, label_len));
            print_line(content, curr_x, curr_y, 0, 0);
        }
        else {
            print_line(label, curr_x, curr_y, OFFSET_LEFT(partial_i, label_len), OFFSET_RIGHT(partial_i, label_len));

            for (i = 1; i <= used_rows; i ++) {
                strncpy(partial_str, content, partial_i);
                partial_str[partial_i] = '\0';

                print_line(partial_str, curr_x, curr_y, OFFSET_LEFT(partial_i, strlen(partial_str)), OFFSET_RIGHT(partial_i, strlen(partial_str)));
                content += partial_i;
            }
        }
    }

    printf("\n");  /* creates new line if no more rows available in terminal */
    *curr_y += 1;
}

void print_field(const char *label, const char *content, int newline) {
    size_t label_len;
    size_t content_len;
    size_t total_rows;
    size_t max_len;
    size_t term_cols;
    size_t term_rows;
    size_t curr_x;
    size_t curr_y;
    size_t initial_y;
    if (NULL == label || NULL == content) return;

    label_len = strlen(label);
    content_len = strlen(content);

    get_terminal_size(&term_cols, &term_rows);

    if (0 != get_cursor_position(&curr_x, &curr_y)) {
        raise_error(CURSOR_POSITION_ERROR, 0, UNCOMPATIBLE_TERMINAL_HINT);
        return;
    }

    initial_y = curr_y; 

    max_len = (2 * strlen(VERTICAL_BORDER)) + (2 * strlen(MARGIN));
    max_len += MAX(label_len, content_len);

    total_rows = USED_ROWS(label, content);
    
    /* total_rows are the current total_rows num. but the previous total rows used are needed to correctly set the cursor */
    if ((curr_x + max_len) > term_cols || ((curr_x + max_len)) > MAX_X || newline) move_to_next_line(&curr_x, &curr_y, prev_used_rows);

    if ((Y_DEVIATION(total_rows) + curr_y) > term_rows) initial_y = term_rows - Y_DEVIATION(total_rows);
    else initial_y = curr_y;

    max_len = PRINTABLE_CHARS(max_len, term_cols, MAX_X);
    print_horizontal_border(max_len, &curr_x, &curr_y);
    print_value(label, content, &curr_x, &curr_y, max_len);
    print_horizontal_border(max_len, &curr_x, &curr_y);

    curr_y = initial_y;
    curr_x += max_len - 1;  /* -1 because it has to rewrite the last vertical line of the previous printed field */
    MOVE_CURSOR(curr_x, curr_y);

    prev_used_rows = total_rows;
}

void print_additional_info(const char *info) { if (NULL != info) printf(YELLOW "(%s)\n" RESET_COLOR, info); }