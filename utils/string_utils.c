#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

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

void lower_str(char *str) {
    while ('\0' != *str) {
        *str = tolower(*str);
        str += 1;
    }
}

void copy_str_n(char **dest, char *src, int n) {
    *dest = (char *)malloc(n + 1);
    if (NULL == *dest) raise_error(NULL_POINTER, 1, NULL, "char **dest", __FILE__);
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
    if (NULL == result) raise_error(NULL_POINTER, 1, NULL, "char *result", __FILE__);

    memset(result, '\0', new_index + 1);
    strncpy(result, new_str, new_index);

    return result;
}

long str_to_num(char *str) {
    char *end;
    long num = 0;
    
    if (NULL != str) num = strtol(str, &end, 10);
    return num;
}

char *str_concat(char **str_arr, char *prefix, char *separator, size_t n_str) {
    int i;
    int total_len = 0;

    for (i = 0; i < n_str; i ++) total_len += strlen(str_arr[i]);
    total_len += (n_str * strlen(prefix)) + ((n_str - 1) * strlen(separator)) + 1;
    /* total_len = (string amount times prefix length) + (string amount - 1 times separator length) + 1 (null terminator) */

    char *new_str = (char *)malloc(sizeof(char) * total_len); /* NULL terminator included */
    if (NULL == new_str) raise_error(NULL_POINTER, 1, NULL, "new_str", __FILE__);

    new_str[0] = '\0';
    for (i = 0; i < n_str; i ++) {
        if (NULL != prefix) strcat(new_str, prefix);
        strcat(new_str, str_arr[i]);
        if (NULL != separator) strcat(new_str, separator);
    }
    new_str[strlen(new_str) - 1] = '\0';

    return new_str;
}

char getch() {
	struct termios oldt, newt;
    char ch;
    
    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    
    // Disable buffered input and echoing
    newt.c_lflag &= ~(ICANON | ECHO); 
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void delete_char(char *str, int pos) {  /* deleting by shifting every other char */
    int i;
    int len = strlen(str);

    if (pos == len) return;
    if (pos < 0) {
        raise_error(NEGATIVE_BUFFER_INDEX, 0, NULL, pos);
        return;
    }

    for (i = pos; i < len; i ++) str[i] = str[i + 1];
}

void push_char(char *str, int buffer_size, int pos, char c) {
    int i;
    int len = strlen(str);

    if ((len + 1) > buffer_size) {
        raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, buffer_size);
        return;
    }

    for (i = (len - 1); i >= pos; i --) str[i + 1] = str[i];
    str[pos] = c;    
}