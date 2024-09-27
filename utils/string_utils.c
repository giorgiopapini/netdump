#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string_utils.h"

int find_word_len(char *sentence, int word_pos) {
    int i;

    for (i = 0; i < strlen(sentence) + 1; i ++) {
        if (' ' == sentence[i] || "\n" == sentence[i]) {
            if (word_pos > 0) word_pos --;
            else return i;
        }
    }
    return strlen(sentence);
}

void copy_str_n(char **dest, char *src, int n) {
    *dest = (char *)malloc(n + 1);
    memset(*dest, '\0', n + 1);
    strncpy(*dest, src, n);
    remove_spaces(src);
    /* add trim functionality and change function name (from "copy_str_n" to something else) */
}

void remove_spaces(char *str) {
    char new_str[strlen(str) + 1];
    size_t new_index = 0;
    char *start = str;
    char *end;
    
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
    printf("new_index: %d,\n", new_index);
    printf("len: %d", strlen(new_str));
    printf("\nNEW_STR:%s\n", new_str);
    /* check eventual unnecessary leading and trailing whitespaces */
}