#include "string_utils.h"

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "../status_handler.h"


size_t find_word_len(char *sentence, int word_pos) {
    size_t i;

    CHECK_NULL_EXIT(sentence);
    for (i = 0; i < strlen(sentence) + 1; i ++) {
        if (' ' == sentence[i] || '\n' == sentence[i]) {
            if (word_pos > 0) word_pos --;
            else return i;
        }
    }
    return strlen(sentence);
}

size_t count_words(const char *str, size_t len) {
    size_t i, count = 0;
    int in_word = 0;
    
    CHECK_NULL_EXIT(str);
    for (i = 0; i < len; i ++) {
        if (isspace(str[i])) in_word = 0;
        else if (!in_word) {
            in_word = 1;
            count ++;
        }
    }
    return count;
}

char *get_filename(char *path) {
    char *filename;
    char *filename_win;
    if (NULL == path) return NULL;
    
    filename = strrchr(path, '/');  /* filename is a pointer to the old string */
    filename_win = strrchr(path, '\\');

    if (filename_win && (!filename || filename_win > filename))
        filename = filename_win;

    return (filename) ? filename + 1 : path;
}

int has_shared_lib_ext(const char *filename) {
    const char *ext;
    
    CHECK_NULL_EXIT(filename);
    ext = strrchr(filename, '.');
    return ext && (0 == strcmp(ext, ".so") || 0 == strcmp(ext, ".dylib"));
}

void lower_str_except_interval(char *str, char interval_symbol) {
    int locked = 0;

    CHECK_NULL_EXIT(str);
    while ('\0' != *str) {
        if (*str == interval_symbol) locked = !locked;
        if (0 == locked) *str = (char)tolower((unsigned char)*str);
        str += 1;
    }
}

void copy_str_n(char **dest, char *src, size_t n) {
    CHECK_NULL_EXIT(dest);
    *dest = (char *)malloc(n + 1);
    CHECK_NULL_EXIT(*dest);

    memset(*dest, '\0', n + 1);
    strncpy(*dest, src, n);
}

char * get_trimmed_str(char *str, size_t str_len) {
    char new_str[str_len + 1];
    size_t new_index = 0;
    char *start = str;
    char *end;
    char *result;

    if (NULL == str) return NULL;
    
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

    if (new_index > 0 && new_str[new_index - 1] == ' ') new_str[new_index - 1] = '\0';

    if (' ' == new_str[0] && new_index > 0) new_index --;
    result = (char *)malloc(new_index  + 1);
    CHECK_NULL_EXIT(result);

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

void uint_to_bin_str(char *str, uint64_t num, size_t dest_str_size) {
    size_t i;
    
    CHECK_NULL_EXIT(str);

    if (dest_str_size > (sizeof(num) * 8 + 1)) {
        raise_error(BUFFER_OVERFLOW_ERROR, 1, NULL, __FILE__, dest_str_size);
        return;
    }

    for (i = dest_str_size - 2; i < dest_str_size - 1; --i) {
        str[i] = (num & 1) + '0';
        num >>= 1;
    }
    str[dest_str_size - 1] = '\0';
}

char *str_concat(const char **str_arr, const char *prefix, const char *separator, size_t n_str) {
    char *new_str;
    size_t total_len;
    size_t prefix_len;
    size_t separator_len;
    size_t i;

    CHECK_NULL_EXIT(prefix);
    CHECK_NULL_EXIT(separator);

    total_len = 0;
    prefix_len = prefix ? strlen(prefix) : 0;
    separator_len = separator ? strlen(separator) : 0;

    if (0 == n_str || NULL == str_arr) return NULL;

    for (i = 0; i < n_str; i ++) {
        if (NULL == str_arr[i]) return NULL;
        total_len += strlen(str_arr[i]) + prefix_len;
    }
    if (n_str > 1) {
        total_len += separator_len * (n_str - 1);
    }
    total_len ++; /* For null terminator */

    new_str = (char *)malloc(total_len);
    CHECK_NULL_EXIT(new_str);

    new_str[0] = '\0';
    for (i = 0; i < n_str; i ++) {
        if (prefix) strcat(new_str, prefix);
        strcat(new_str, str_arr[i]);
        if (separator && i < n_str - 1) strcat(new_str, separator);
    }

    return new_str;
}

int getch(void) {
	struct termios oldt, newt;
    int oldf;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch;
}

void delete_char(char *str, size_t pos) {  /* deleting by shifting every other char */
    size_t len;
    size_t i;
    
    CHECK_NULL_EXIT(str);
    len = strlen(str);

    if (pos == len) return;
    for (i = pos; i < len; i ++) str[i] = str[i + 1];
}

void push_char(char *str, size_t buffer_size, size_t pos, int c) {
    size_t len;
    size_t i;

    CHECK_NULL_EXIT(str);
    len = strlen(str);

    if (c < CHAR_MIN || c > CHAR_MAX) {
        raise_error(INT_TO_CHAR_CAST_ERROR, 0, NULL);
        return;
    }    
    
    if (len >= buffer_size - 1) {
        raise_error(BUFFER_OVERFLOW_ERROR, 0, NULL, __FILE__, buffer_size);
        return;
    }

    if (pos >= len) {
        str[pos] = (char)c;
        str[pos + 1] = '\0';
    }
    else {
        for (i = len; i > pos; i--) str[i] = str[i - 1];
        str[pos] = (char)c;
    }
}