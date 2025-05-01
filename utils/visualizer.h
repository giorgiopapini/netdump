#ifndef VISUALIZER_H
#define VISUALIZER_H

#define MARGIN "  "
#define HORIZONTAL_BORDER "-"
#define VERTICAL_BORDER "|"
#define JUNCTION "*"

#define CONT_HORIZONTAL_BORDER "─"
#define CONT_VERTICAL_BORDER "│"
#define CONT_JUNCTION "•"

#define USED_ROWS(label, content)           (calc_rows(label) + calc_rows(content))

typedef enum {
    BORDER_STANDARD,
    BORDER_MODERN
} border_type;

size_t calc_rows(const char *str);
int get_cursor_position(size_t *col, size_t *row);
void move_to_next_line(size_t *curr_x, size_t *curr_y, size_t used_rows);
void start_printing(void);
void end_printing(void);
void print_field(const char *label, const char *content, int newline);
void print_additional_info(const char *info);

#endif