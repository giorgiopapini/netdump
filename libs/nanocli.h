#ifndef NANOCLI_LIB_H
#define NANOCLI_LIB_H

#include <stddef.h>

#define NCLI_DEFAULT_PROMPT                "nanocli > "
#define NCLI_DEFAULT_MASKED_CHAR           '*'
#define NCLI_DEFAULT_MAX_INPUT_LEN         1024
#define NCLI_DEFAULT_HISTORY_MAX_SIZE      1024


char *nanocli(const char *prompt, size_t max_str_len);
char *nanocli_ask(const char *question, const size_t max_len, const int masked);
void nanocli_echo(const char *str);

#endif
