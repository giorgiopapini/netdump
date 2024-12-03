#include <stdio.h>
#include <string.h>

#include "visualizer.h"

#define MOVE_CURSOR(x, y)                   (printf("\033[%d;%dH", (y), (x)))
#define MAX(x, y)                           ((x > y) ? x : y)
#define MIN(x, y)                           ((x > y) ? y : x)

void print_horizontal_border(int len, int *curr_x, int *curr_y) {
    int i;
    len -= 2 * strlen(JUNCTION);  /* this exclude space needed for the junctions inside the for loop */

    MOVE_CURSOR(*curr_x, *curr_y);
    printf(JUNCTION);
    for (i = 0; i < len; i ++) {
        /* check if you are not overriding a JUNCTION string */
        printf(HORIZONTAL_BORDER);
    }
    printf(JUNCTION);
}

void print_value(char *val, int *curr_x, int *curr_y, int offset_left, int offset_right) {
    *curr_y += 1; 
    MOVE_CURSOR(*curr_x, *curr_y);
    printf(VERTICAL_BORDER MARGIN);
    for (; offset_left > 0; offset_left --) printf(" ");
    printf("%s", val);
    for (; offset_right > 0; offset_right --) printf(" ");
    printf(MARGIN VERTICAL_BORDER);
}

void print_field(char *label, char *content, int *curr_x, int *curr_y) {
    int initial_y = *curr_y;
    int label_len = strlen(label);
    int content_len = strlen(content);
    int off_left;
    int off_right;
    int max_len;

    int min = MIN(label_len, content_len);
    int max = MAX(label_len, content_len);

    max_len = (2 * strlen(VERTICAL_BORDER)) + (2 * strlen(MARGIN));
    max_len += max;

    off_left = (max - min) / 2;
    off_right = ((max % min) == 0) ? (max - min) / 2 :((max - min) / 2) + 1;

    print_horizontal_border(max_len, curr_x, curr_y);
    if (label_len > content_len) {
        print_value(label, curr_x, curr_y, 0, 0);
        print_value(content, curr_x, curr_y, off_left, off_right);
    }
    else {
        print_value(label, curr_x, curr_y, off_left, off_right);
        print_value(content, curr_x, curr_y, 0, 0);
    }
    *curr_y += 1;
    print_horizontal_border(max_len, curr_x, curr_y);

    *curr_y = initial_y;
    *curr_x += max_len - 1;  /* -1 because it has to rewrite the last vertical line of the previous printed field */
}