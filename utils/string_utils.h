#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdint.h>

size_t find_word_len(char *sentence, int word_pos);    /* first word len? -> word_pos should be 0 (first index) */
char *get_filename(char *path);
int has_shared_lib_ext(const char *filename);
void copy_str_n(char **dest, char *src, size_t end_index);
char * get_trimmed_str(char *str, size_t str_len);
void lower_str_except_interval(char *str, char interval_symbol);  /* interval_symbol is the opening and closing char that delimitates the interval */
long str_to_num(char *str);
void uint_to_bin_str(char *str, uint64_t num, size_t size);
char *str_concat(const char **str_arr, const char *prefix, const char *separator, size_t n_str);
int getch(void);   /* simulates behaviour of getch() function in <conio.h> for Win systems. Get char without the enter */
void delete_char(char *str, size_t pos);
void push_char(char *str, size_t buffer_size, size_t pos, int c);

#endif