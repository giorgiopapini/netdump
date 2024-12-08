#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <math.h>

#include "visualizer.h"
#include "../status_handler.h"

#define STARTING_X_VAL 1
#define FIELD_MIN_Y_LEN 3
#define MAX_X 80

#define MOVE_CURSOR(x, y)                   (printf("\033[%d;%dH", (y), (x)))
#define MAX(x, y)                           ((x > y) ? x : y)
#define MIN(x, y)                           ((x > y) ? y : x)
#define Y_DEVIATION(rows)                   (rows + 1)  /* considering field rows + bottom border row */


void get_terminal_size(int *cols, int *rows) {
    struct winsize w;
    if (-1 == ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) raise_error(TERMINAL_SIZE_ERROR, 1, NULL);

    *cols = w.ws_col;
    *rows = w.ws_row;
}

int calc_rows(char *str) {
    int cols, rows;
    int max_len = strlen(str) + (2 * strlen(MARGIN));
    get_terminal_size(&cols, &rows);

    return (max_len / MIN(cols, MAX_X)) == 0 ? 1 : (max_len / MIN(cols, MAX_X));
}

int get_cursor_position(int *col, int *row) {
    char buf[32];
    unsigned int i = 0;
    int ret;

    struct termios saved, raw;
    tcgetattr(STDIN_FILENO, &saved);

    raw = saved;
    raw.c_lflag &= ~(ICANON | ECHO);
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
        sscanf(buf + 2, "%d;%d", row, col);
    }
    else return -1;

    return 0;
}

void move_to_next_line(int *curr_x, int *curr_y, int used_rows) {
    if (NULL == curr_x && NULL == curr_y) {
        int x, y;
        get_cursor_position(&x, &y);
        MOVE_CURSOR(0, y + Y_DEVIATION(used_rows));
    }
    else {
        *curr_x = STARTING_X_VAL;
        *curr_y += Y_DEVIATION(used_rows);
        MOVE_CURSOR(*curr_x, *curr_y);
    }
}

void print_horizontal_border(int len, int *curr_x, int *curr_y) {
    int i;
    len -= 2 * strlen(JUNCTION);  /* this exclude space needed for the junctions inside the for loop */

    MOVE_CURSOR(*curr_x, *curr_y);
    printf(JUNCTION);
    for (i = 0; i < len; i ++) printf(HORIZONTAL_BORDER);
    printf(JUNCTION);
}

/* make it support multiple line content, as described in the notes */
void print_value(char *val, int *curr_x, int *curr_y, int offset_left, int offset_right) {
    printf("\n");  /* creates new line if no more rows available in terminal */
    
    *curr_y += 1; 
    MOVE_CURSOR(*curr_x, *curr_y);
    
    printf(VERTICAL_BORDER MARGIN);
    for (; offset_left > 0; offset_left --) printf(" ");
    printf("%s", val);
    for (; offset_right > 0; offset_right --) printf(" ");
    printf(MARGIN VERTICAL_BORDER);
}

void print_field(char *label, char *content, int newline) {
    int label_len = strlen(label);
    int content_len = strlen(content);
    int total_rows;
    int off_left;
    int off_right;
    int max_len;
    int term_cols;
    int term_rows;
    int curr_x;
    int curr_y;
    int initial_y;
    int min;
    int max;

    get_terminal_size(&term_cols, &term_rows);

    if (0 != get_cursor_position(&curr_x, &curr_y)) {
        raise_error(CURSOR_POSITION_ERROR, 0, UNCOMPATIBLE_TERMINAL_HINT);
        return;
    }

    initial_y = curr_y;
    min = MIN(label_len, content_len);
    max = MAX(label_len, content_len);

    max_len = (2 * strlen(VERTICAL_BORDER)) + (2 * strlen(MARGIN));
    max_len += max;

    off_left = (max - min) / 2;
    off_right = ceil(((max % min) == 0) ? ((double)max - (double)min) / (double)2 : (((double)max - (double)min) / (double)2));

    total_rows = USED_ROWS(label, content);

    if ((curr_x + max_len) > term_cols || ((curr_x + max_len)) > MAX_X || newline) move_to_next_line(&curr_x, &curr_y, total_rows);

    if ((Y_DEVIATION(total_rows) + curr_y) > term_rows) initial_y = term_rows - Y_DEVIATION(total_rows);
    else initial_y = curr_y;

    print_horizontal_border(max_len, &curr_x, &curr_y);
    if (label_len > content_len) {
        print_value(label, &curr_x, &curr_y, 0, 0);
        print_value(content, &curr_x, &curr_y, off_left, off_right);
    }
    else {
        print_value(label, &curr_x, &curr_y, off_left, off_right);
        print_value(content, &curr_x, &curr_y, 0, 0);
    }

    printf("\n");  /* creates new line if no more rows available in terminal */
    curr_y += 1;
    print_horizontal_border(max_len, &curr_x, &curr_y);

    curr_y = initial_y;
    curr_x += max_len - 1;  /* -1 because it has to rewrite the last vertical line of the previous printed field */
    MOVE_CURSOR(curr_x, curr_y);
}