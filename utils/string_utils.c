#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
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

    if (' ' == new_str[0]) new_index --;
    result = (char *)malloc(new_index  + 1);
    if (NULL == result) raise_error(NULL_POINTER, 1, NULL, "char *result", __FILE__);

    memset(result, '\0', new_index + 1);
    if (' ' == new_str[0]) strncpy(result, new_str + 1, new_index);  /* does not copy the first char, which is an empty space */
    else strncpy(result, new_str, new_index);

    return result;
}

long str_to_num(char *str) {
    char *end;
    long num = 0;
    
    if (NULL != str) num = strtol(str, &end, 10);
    return num;
}

void uint16_to_bin_str(char *str, uint16_t num, size_t size) {
    for (int i = size - 2; i >= 0; i--) {
        str[i] = (num & 1) + '0';
        num >>= 1;
    }
    str[size - 1] = '\0';
}

char *str_concat(char **str_arr, char *prefix, char *separator, size_t n_str) {
    if (n_str == 0 || str_arr == NULL) return NULL;

    size_t total_len = 0;
    size_t prefix_len = prefix ? strlen(prefix) : 0;
    size_t separator_len = separator ? strlen(separator) : 0;

    for (size_t i = 0; i < n_str; i ++) {
        if (str_arr[i] == NULL) return NULL;
        total_len += strlen(str_arr[i]) + prefix_len;
    }
    if (n_str > 1) {
        total_len += separator_len * (n_str - 1);
    }
    total_len ++; /* For null terminator */

    char *new_str = (char *)malloc(total_len);
    if (new_str == NULL) raise_error(NULL_POINTER, 1, NULL, "new_str", __FILE__);

    new_str[0] = '\0';
    for (size_t i = 0; i < n_str; i ++) {
        if (prefix) strcat(new_str, prefix);
        strcat(new_str, str_arr[i]);
        if (separator && i < n_str - 1) strcat(new_str, separator);
    }

    return new_str;
}

char getch() {
	struct termios oldt, newt;
    int oldf;
    char ch;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echoing
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Save old file descriptor flags and set non-blocking mode
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Try to read a character
    ch = getchar();

    // Restore the old terminal settings and file descriptor flags
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch; // Returns the character, or EOF if no input
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