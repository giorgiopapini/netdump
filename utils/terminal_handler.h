#ifndef TERMINAL_HANDLER_H
#define TERMINAL_HANDLER_H

#include <stddef.h>


void enable_raw_mode(void);
void restore_terminal_mode(void);
void get_terminal_size(size_t *cols, size_t *rows);
int get_cursor_position(size_t *col, size_t *row);

#endif