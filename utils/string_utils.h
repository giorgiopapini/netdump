#ifndef STRING_UTILS_H
#define STRING_UTILS_H

int find_word_len(char *sentence, int word_pos);    /* first word len? -> word_pos should be 0 (first index) */
void copy_str_n(char **dest, char *src, int end_index);
char * get_trimmed_str(char *str);

#endif