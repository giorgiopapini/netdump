#ifndef VISUALIZER_H
#define VISUALIZER_H

#define MARGIN "    "
#define HORIZONTAL_BORDER "-"
#define VERTICAL_BORDER "|"
#define JUNCTION "*"

#define USED_ROWS(label, content)           (calc_rows(label) + calc_rows(content))

int calc_rows(char *str);
int get_cursor_position(int *row, int *col);
void move_to_next_line(int *curr_x, int *curr_y, int used_rows);
void start_printing();
void end_printing();
void print_field(char *label, char *content, int newline);
void print_hdr_info(char *name, char *optional_infos);

#endif