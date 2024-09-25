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
    /* add trim functionality and change function name (from "copy_str_n" to something else) */
}

void trim(char *str) {

}