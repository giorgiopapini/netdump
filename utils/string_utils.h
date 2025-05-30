#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <stdint.h>

size_t find_word_len(char *sentence, int word_pos);    /* first word len? -> word_pos should be 0 (first index) */
size_t count_words(const char *str, size_t len);
int is_abs_path(const char *path);
int get_absolute_path(const char *relative_path, char *abs_path, size_t max_len);
char *get_filename(char *path);
int has_shared_lib_ext(const char *filename);
void copy_str_n(char **dest, char *src, size_t end_index);
char * get_trimmed_str(char *str, size_t str_len);
char *new_lower_str(const char *str);
void lower_str_except_interval(char *str, const char *interval_delim);  /* interval_symbol is the opening and closing char that delimitates the interval */
long str_to_long(char *str);
int long_to_int(long num);
void uint_to_bin_str(char *str, uint64_t num, size_t size);
char *str_concat(const char **str_arr, const char *prefix, const char *separator, size_t n_str);
int getch(void);   /* simulates behaviour of getch() function in <conio.h> for Win systems. Get char without the enter */
void delete_char(char *str, size_t pos);
void push_char(char *str, size_t buffer_size, size_t pos, int c);

#endif