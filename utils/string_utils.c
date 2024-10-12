#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_utils.h"
#include "../status_handler.h"

int find_word_len(char *sentence, int word_pos) {
    int i;

    for (i = 0; i < strlen(sentence) + 1; i ++) {
        if (' ' == sentence[i] || '\n' == sentence[i]) {
            if (word_pos > 0) word_pos --;
            else return i;
        }
    }
    return strlen(sentence);
}

void copy_str_n(char **dest, char *src, int n) {
    *dest = (char *)malloc(n + 1);
    if (NULL == *dest) raise(NULL_POINTER, 1, "char **dest", __FILE__);
    memset(*dest, '\0', n + 1);
    strncpy(*dest, src, n);
}

char * get_trimmed_str(char *str) {
    char new_str[strlen(str) + 1];
    size_t new_index = 0;
    char *start = str;
    char *end;
    char *result;
    
    while (*start != '\0') {
        if (*start != ' ') {
            new_str[new_index] = *start;
            start ++;
        }
        else {
            end = start + 1;
            while (*end == ' ') end ++;
            new_str[new_index] = ' ';
            start = end;
        }
        new_index ++;
    }

    if (new_str[new_index - 1] == ' ') new_str[new_index - 1] = '\0';

    result = (char *)malloc(new_index  + 1);
    if (NULL == result) raise(NULL_POINTER, 1, "char *result", __FILE__);

    memset(result, '\0', new_index + 1);
    strncpy(result, new_str, new_index);

    return result;
}