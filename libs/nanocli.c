#include "nanocli.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ctype.h>
#include <termios.h>

#define ARROW_UP_KEY 'A'
#define ARROW_DOWN_KEY 'B'
#define ARROW_RIGHT_KEY 'C'
#define ARROW_LEFT_KEY 'D'
#define CANC_KEY '3'
#define TILDE_KEY '~'

struct ncli_cursor {
    size_t x;
    size_t y;
};

struct ncli_line {
    char *content;
    size_t len;  /* excluding NULL terminator */
    size_t cap;  /* allocated size (max_len) */
};

struct ncli_history {
    struct ncli_line **entries;  /* dinamically allocated ncli_line pointers */
    size_t curr;
    size_t len;
    size_t cap;
};

struct ncli_cli_state {
    const char *prompt;  /* should be null terminated */
    struct ncli_line **p_line;
    struct ncli_cursor *curs;
    size_t term_cols;
};

typedef enum {
    NCLI_EXIT = -1,
    NCLI_NOP = 0,  /* will not print '\n' after callback */
    NCLI_CONTINUE,  /* will add a '\n' after callback, used to render content printed in the callback */
    NCLI_SEND_COMMAND
} ncli_stat_code;

typedef enum {
	CTRL_A = 1,
	CTRL_B = 2,
	CTRL_C = 3,
	CTRL_D = 4,
	CTRL_E = 5,
	CTRL_F = 6,
	CTRL_H = 8,
	TAB = 9,
    NEWLINE_KEY = 10,
	CTRL_K = 11,
	CTRL_L = 12,
	CARR_RET_KEY = 13,
	CTRL_N = 14,
	CTRL_P = 16,
	CTRL_T = 20,
	CTRL_U = 21,
	CTRL_W = 23,
	ESC_KEY = 27,
	BACKSPACE_KEY = 127
} ncli_keys;

/* ================= functions related to line management ================== */
struct ncli_line *_ncli_create_line(const size_t max_len);
static void _ncli_remove_char(struct ncli_line *line, const size_t target_index);
static void _ncli_delete_to_end(struct ncli_line *line, const size_t start_index);
static void _ncli_delete_to_start(struct ncli_line *line, const size_t end_index);
static void _ncli_delete_word(struct ncli_line *line, const size_t curr_index);
static void _ncli_add_char(struct ncli_line *line, const size_t target_index, const char new_char);
static void _ncli_copy_line(struct ncli_line *dest, const struct ncli_line *src);
static int _ncli_line_is_empty(const struct ncli_line *line);
static int _ncli_line_equal(const struct ncli_line *line1, const struct ncli_line *line2);
static void _ncli_clean_line(struct ncli_line *line);
static void _ncli_free_line(struct ncli_line *line);
/* ========================================================================= */
/* ================ functions related to history management ================ */
static struct ncli_history *_ncli_create_history(const size_t max_len);
static void _ncli_add_entry(struct ncli_history *history, const struct ncli_line *new_line);
static void _ncli_free_history(struct ncli_history **p_history);

struct ncli_history *glob_history = NULL;
/* ========================================================================= */
/* ========================== terminal management ========================== */
static void _get_terminal_size(size_t *cols, size_t *rows);
void _clear_nanocli_screen(void);
static void _enable_raw_mode(void);
static void _restore_terminal_mode(void);

static struct termios orig_termios;
static int termios_saved = 0;
static int raw_modncli_on = 0;
/* ========================================================================= */

static struct ncli_cli_state *_create_ncli_cli_state(const char *prompt, const size_t max_linncli_size);
static void _ncli_free_cli_state(struct ncli_cli_state *cli);
static int _is_cli_state_valid(struct ncli_cli_state *cli);
static size_t _get_line_index_from_curs(struct ncli_cli_state *cli);
/* double pointer is needed because these functions free *p_dest and loads the ncli_line retrieved from history */
static void _set_line_to_history_curr(struct ncli_cli_state *cli, struct ncli_history *history);
static void _move_cursor_last_line(struct ncli_cli_state *cli, const char pressed_key);
static void _enter(struct ncli_cli_state *cli, struct ncli_history *history, const char c);
static void _up_arrow(struct ncli_cli_state *cli, struct ncli_history *history);
static void _down_arrow(struct ncli_cli_state *cli, struct ncli_history *history);
static void _right_arrow(struct ncli_cli_state *cli);
static void _left_arrow(struct ncli_cli_state *cli);
static void _canc(struct ncli_cli_state *cli);
static void _backspace(struct ncli_cli_state *cli);
static void _literal(struct ncli_cli_state *cli, char *c);
static void _ctrl_k(struct ncli_cli_state *cli);
static void _ctrl_t(struct ncli_cli_state *cli);
static void _ctrl_u(struct ncli_cli_state *cli);
static void _ctrl_w(struct ncli_cli_state *cli);
char *_get_line(const char *prompt, const size_t max_len, struct ncli_history *history, const int masked);
static void _clean_line(struct ncli_cli_state *cli);
static void _write_line(struct ncli_cli_state *cli, const int masked);
static ncli_stat_code _handle_display(
    struct ncli_cli_state *cli,
    struct ncli_history *history,
    char *c,
    const int masked
);
static ncli_stat_code _handle_char_input(struct ncli_cli_state *cli, struct ncli_history *history, const int masked);


/* ================= functions related to line management ================== */
struct ncli_line *_ncli_create_line(const size_t max_len) {
    struct ncli_line *new_line = malloc(sizeof *new_line);
    if (NULL == new_line) return NULL;

    new_line->content = calloc(max_len, sizeof *(new_line->content));
    if (NULL == new_line->content) {
        free(new_line);
        return NULL;
    }

    new_line->cap = max_len;
    new_line->len = 0;
    return new_line;
}

void _ncli_remove_char(struct ncli_line *line, const size_t target_index) {
    size_t i;

    if (NULL == line) return;
    if (NULL == line->content) return;
    if (target_index > line->len - 1) return;

    for (i = target_index; i < line->len; i ++)
        line->content[i] = line->content[i + 1];
    line->len --;
}

static void _ncli_delete_to_end(struct ncli_line *line, const size_t start_index) {
    size_t i;

    if (NULL == line || NULL == line->content) return;
    for (i = start_index; i < line->len; i ++) line->content[i] = '\0';
    line->len -= (line->len - start_index);
}

static void _ncli_delete_to_start(struct ncli_line *line, const size_t end_index) {
    size_t new_len;
    if (NULL == line || NULL == line->content || line->len <= 0) return;
    new_len = line->len - (end_index + 1);
    memmove(line->content, line->content + end_index + 1, new_len);
    line->content[new_len] = '\0';
    line->len = new_len;
}

static void _ncli_delete_word(struct ncli_line *line, const size_t curr_index) {
    size_t i = curr_index;
    if (0 == line->len) return;

    while (i > 0 && isspace((unsigned char)line->content[i - 1])) i--;    
    while (i > 0 && !isspace((unsigned char)line->content[i - 1])) i--;

    memmove(&line->content[i], &line->content[curr_index], line->len - curr_index);    
    line->len -= (curr_index - i);
    line->content[line->len] = '\0';
}

void _ncli_add_char(struct ncli_line *line, const size_t target_index, const char new_char) {
    size_t i;

    if (target_index > line->cap - 1) return;
    if (line == NULL || line->content == NULL) return;
    if (line->len >= line->cap - 1) return;

    for (i = line->len; i > target_index; i --)
        line->content[i] = line->content[i - 1];

    line->content[target_index] = new_char;
    line->len ++;
    line->content[line->len] = '\0';
}

void _ncli_copy_line(struct ncli_line *dest, const struct ncli_line *src) {
    size_t i;

    if (NULL == dest || NULL == dest->content || NULL == src || NULL == src->content)
        return;

    dest->len = src->len;
    dest->cap = src->cap;

    for (i = 0; i < src->len; i ++) dest->content[i] = src->content[i];
    dest->content[dest->len] = '\0';
}

int _ncli_line_is_empty(const struct ncli_line *line) {
    size_t i;

    if (NULL == line) return 1;
    if (NULL == line->content) return 1;

    for (i = 0; i < line->len; i ++)
        if (!isspace((unsigned char)line->content[i])) return 0;
    return 1;
}

int _ncli_line_equal(const struct ncli_line *line1, const struct ncli_line *line2) {
    size_t i;

    if (NULL == line1 || NULL == line2) return 0;
    if (line1->len != line2->len) return 0;

    if (NULL == line1->content && NULL == line2->content) return 1;
    else if (NULL == line1->content || NULL == line2->content) return 0;

    for (i = 0; i < line1->len; i ++)
        if (line1->content[i] != line2->content[i]) return 0;
    return 1;
}

void _ncli_clean_line(struct ncli_line *line) {
    if (NULL == line) return;
    if (NULL == line->content) return;
    
    line->len = 0;
    memset(line->content, 0x0, line->cap);
}

void _ncli_free_line(struct ncli_line *line) {
    if (NULL == line) return;

    if (NULL == line->content) {
        free(line);
        return;
    }
    free(line->content);
    free(line);
}
/* ========================================================================= */
/* ================ functions related to history management ================ */
struct ncli_history *_ncli_create_history(const size_t max_len) {
    struct ncli_history *new_history = malloc(sizeof *new_history);
    if (NULL == new_history) return NULL;

    new_history->entries = calloc(max_len, sizeof *(new_history->entries));
    if (NULL == new_history->entries) {
        free(new_history);
        return NULL;
    }

    new_history->cap = max_len;
    new_history->len = 0;
    new_history->curr = 0;
    return new_history;
}

void _ncli_add_entry(struct ncli_history *history, const struct ncli_line *new_line) {
    /* Makes a copy of new_line and appends it to history */
    size_t i;
    struct ncli_line *copy_str;

    if (
        NULL == history ||
        NULL == history->entries ||
        NULL == new_line
    ) return;

    if (_ncli_line_is_empty(new_line)) return;
    if (history->len > 0) {
        if (_ncli_line_equal(new_line, history->entries[history->len - 1])) return;
    }

    copy_str = _ncli_create_line(new_line->cap);
    if (NULL == copy_str) return;

    _ncli_copy_line(copy_str, new_line);

    /* remember: when an entry is added, history->curr always points to the last added element */
    if (history->len < history->cap) {
        history->len ++;
        history->entries[history->len - 1] = copy_str;
        history->curr = history->len - 1;
    }
    else {
        /* deallocate first entry than shift eveything to the left and add entry in the last element at (history->cap - 1) */
        _ncli_free_line(history->entries[0]);
        for (i = 1; i < history->cap; i ++)
            history->entries[i -1] = history->entries[i];
        history->entries[history->cap - 1] = copy_str;
        history->curr = history->cap - 1;
    }
}

void _ncli_free_history(struct ncli_history **p_history) {
    size_t i;

    if (NULL == p_history || NULL == *p_history || (*p_history)->len > (*p_history)->cap) return;
    if (NULL == (*p_history)->entries) {
        free(*p_history);
        *p_history = NULL;
        return;
    }

    for (i = 0; i < (*p_history)->len; i ++)
        if (NULL != (*p_history)->entries[i]) _ncli_free_line((*p_history)->entries[i]);

    free((*p_history)->entries);
    free(*p_history);
    *p_history = NULL;
}
/* ========================================================================= */
/* ========================== terminal management ========================== */
void _clear_nanocli_screen(void) {
    if (write(STDOUT_FILENO,"\x1b[H\x1b[2J",7) <= 0) return;
}

void _enable_raw_mode(void) {
    struct termios raw;

    if (-1 == tcgetattr(STDIN_FILENO, &orig_termios)) return;
    termios_saved = 1;

    raw = orig_termios;
    raw.c_iflag &= (tcflag_t)~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= (tcflag_t)~(OPOST);
    raw.c_cflag |= (tcflag_t)(CS8);
    raw.c_lflag &= (tcflag_t)~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &raw)) return;
    raw_modncli_on = 1;
}

void _restore_terminal_mode(void) {
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        raw_modncli_on = 0;
    }
}

void _get_terminal_size(size_t *cols, size_t *rows) {
    struct winsize w;
    if (-1 == ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) return;

    if (NULL != cols) *cols = w.ws_col;
    if (NULL != rows) *rows = w.ws_row;
}
/* ========================================================================= */
/* ============================ CLI management ============================= */
static struct ncli_cli_state *_create_ncli_cli_state(const char *prompt, const size_t max_linncli_size) {
    struct ncli_cli_state *new_state = malloc(sizeof *new_state);
    if (NULL == new_state) return NULL;

    new_state->p_line = malloc(sizeof *new_state->p_line);
    if (NULL == new_state->p_line) return NULL;
    (*new_state->p_line) = _ncli_create_line(max_linncli_size + 1);

    new_state->curs = malloc(sizeof *new_state->curs);
    if (NULL == new_state->curs) return NULL;
    new_state->curs->x = 0;
    new_state->curs->y = 0;

    new_state->prompt = prompt;
    _get_terminal_size(&new_state->term_cols, NULL);
    
    return new_state;
}

static void _ncli_free_cli_state(struct ncli_cli_state *cli) {
    if (NULL == cli) return;
    if (NULL != cli->p_line) _ncli_free_line(*cli->p_line);
    if (NULL != cli->p_line) free(cli->p_line);
    if (NULL != cli->curs) free(cli->curs);
    if (NULL != cli) free(cli);
}

static int _is_cli_state_valid(struct ncli_cli_state *cli) {
    if (NULL == cli) return 0;
    if (NULL == cli->curs) return 0;
    if (NULL == cli->p_line || NULL == *cli->p_line) return 0;
    return 1;
}

static size_t _get_line_index_from_curs(struct ncli_cli_state *cli) {
    size_t linncli_index = 0;
    size_t prompt_len;
    
    if (NULL == cli || NULL == cli->curs) return 0;
    prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;

    if (cli->curs->y > 0)
        linncli_index = (cli->curs->y * cli->term_cols) + cli->curs->x - prompt_len;
    else linncli_index = (cli->curs->x - prompt_len);
    return linncli_index;
}

static void _set_line_to_history_curr(struct ncli_cli_state *cli, struct ncli_history *history) {
    /* _ncli_free_line(*p_dest) --> than allocate new memory for storing the retrieved entry from history.
    the freshly allocated memory will than be automatically deallocated at the end of everything */
    struct ncli_line *res;
    size_t used_rows;
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;
    if (0 == history->len) return;

    res = history->entries[history->curr];
    _ncli_free_line(*cli->p_line);
    if (NULL != res) *cli->p_line = _ncli_create_line(res->cap);
    _ncli_copy_line(*cli->p_line, res);

    /* settings the cursor to be at the end of the new string */
    used_rows = (prompt_len + res->len + cli->term_cols - 1) / cli->term_cols;
    cli->curs->y = (used_rows > 0) ? used_rows - 1 : 0;
    cli->curs->x = (res->len + prompt_len) % cli->term_cols;
}

static void _move_cursor_last_line(struct ncli_cli_state *cli, const char pressed_key) {
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;
    size_t used_rows = (prompt_len + (*cli->p_line)->len + cli->term_cols - 1) / cli->term_cols;
    size_t movncli_down = (used_rows - 1) - cli->curs->y;
    char buf[32];
    int len;

    if (cli->curs->x == cli->term_cols && ARROW_LEFT_KEY == pressed_key) movncli_down --;
    
    if (movncli_down > 0) {
        len = snprintf(buf, sizeof buf, "\033[%ldB\r", movncli_down);
        if (write(STDOUT_FILENO, buf, (size_t)len) < 0) return;  /* len can't be negative */
    }

    if (used_rows > 1)
        len = snprintf(buf, sizeof buf, "\r\033[%ldC", ((*cli->p_line)->len + prompt_len) % (cli->term_cols * (used_rows - 1)));
    else len = snprintf(buf, sizeof buf, "\r\033[%ldC", ((*cli->p_line)->len + prompt_len));

    if (write(STDOUT_FILENO, buf, (size_t)len) < 0) return;  /* len can't be negative */
}

static void _enter(struct ncli_cli_state *cli, struct ncli_history *history, const char c) {
    if (NULL != history) {
        _ncli_add_entry(history, *cli->p_line);
        history->curr = (history->len > 0) ? history->len - 1 : 0;
    }
    (*cli->p_line)->content[(*cli->p_line)->len] = '\0';
    _move_cursor_last_line(cli, c);
    if (write(STDOUT_FILENO, "\r\n", 2) < 0) return;
}

static void _up_arrow(struct ncli_cli_state *cli, struct ncli_history *history) {
    if (NULL == history) return;
    if (history->curr == history->len) {
        _ncli_clean_line(*cli->p_line);
        cli->curs->x = 0;
        cli->curs->y = 0;
    }
    else _set_line_to_history_curr(cli, history);
    
    if (0 == history->curr) history->curr = history->len;
    else history->curr --;
}

static void _down_arrow(struct ncli_cli_state *cli, struct ncli_history *history) { 
    cli->curs->x = 0;
    cli->curs->y = 0;
    
    if (NULL == history) return;
    if (history->curr == history->len) history->curr = 0;
    if (history->curr == history->len - 1) {
        _ncli_clean_line(*cli->p_line);
        return;
    }
    
    history->curr ++;
    if (_ncli_line_equal(*cli->p_line, history->entries[history->curr]))
        history->curr ++;
    
    if (history->curr < history->len)
        _set_line_to_history_curr(cli, history);
    else {
        if (history->len > 0) history->curr = history->len - 1;
        else history->curr = 0;
        _ncli_clean_line(*cli->p_line);
    }
}

static void _right_arrow(struct ncli_cli_state *cli) {
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;

    if (cli->curs->y <= SIZE_MAX / cli->term_cols && prompt_len <= SIZE_MAX - (*cli->p_line)->len) {
        if (cli->curs->y * cli->term_cols + cli->curs->x < (*cli->p_line)->len + prompt_len) {
            /* -1 is for the condition to be valid, adding -1 is because
            curs->x starts from 0 and term_cols starts from 1 --> so it is -2 */
            if (cli->curs->x > cli->term_cols - 2) {
                cli->curs->x = 0;
                cli->curs->y ++;
            }
            else cli->curs->x ++;
        }
    }
}

static void _left_arrow(struct ncli_cli_state *cli) {
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;
    if (0 == (*cli->p_line)->len) return;

    if (cli->curs->y > 0) {
        if (cli->curs->x == 0) {
            cli->curs->x = cli->term_cols - 1;
            cli->curs->y --;
        }
        else cli->curs->x --;
    }
    else if (cli->curs->x > prompt_len) cli->curs->x --;
}

static void _canc(struct ncli_cli_state *cli) {
    size_t abs_x = 0;
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;
    size_t used_rows = (prompt_len + (*cli->p_line)->len + cli->term_cols - 1) / cli->term_cols;

    if ((*cli->p_line)->len > 0 && cli->curs->x < cli->term_cols && used_rows >= 1) {
        if (0 == cli->curs->y) abs_x = cli->curs->x;
        else abs_x = cli->curs->x + (cli->curs->y * cli->term_cols);

        /* this condition prevents canc beyond string end */
        if (abs_x - prompt_len < (*cli->p_line)->len) _ncli_remove_char(*cli->p_line, abs_x - prompt_len);
    }
}

static void _backspace(struct ncli_cli_state *cli) {
    int exec_backspace = 1;
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;
    if (0 >= (*cli->p_line)->len) return;  /* nothing to delete, return */

    if (cli->curs->y > 0) {
        if (cli->curs->x == 0) {
            cli->curs->y --;
            cli->curs->x = cli->term_cols - 1;
        }
        else cli->curs->x --;
    }
    else if (cli->curs->y == 0) {
        if (cli->curs->x > prompt_len) cli->curs->x --;
        else exec_backspace = 0;
    }
    if (exec_backspace) _canc(cli);

    if (0 == cli->curs->x && 0 < cli->curs->y) {
        cli->curs->y --;
        cli->curs->x = cli->term_cols;
    }
}

static void _literal(struct ncli_cli_state *cli, char *c) {
    size_t real_index = 0;
    size_t prompt_len = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;

    if ((*cli->p_line)->len < (*cli->p_line)->cap - 1) {
        if (0 == cli->curs->y) real_index = cli->curs->x - prompt_len;
        else {
            real_index = cli->term_cols - prompt_len;  /* first row (excludes the prompt str length) */
            if (cli->curs->y > 1) real_index += (cli->curs->y - 1) * cli->term_cols;  /* length rows in the middle */
            real_index += cli->curs->x;
        }
        _ncli_add_char(*cli->p_line, real_index, *c);  /* curs->x - prompt_len is valid only for the first line */

        if (cli->curs->x > cli->term_cols - 1) {
            cli->curs->x = 1;
            cli->curs->y ++;
        }
        else cli->curs->x ++;
    }
}

static void _ctrl_k(struct ncli_cli_state *cli) {
    size_t real_index = _get_line_index_from_curs(cli);
    _ncli_delete_to_end(*cli->p_line, real_index);
}

static void _ctrl_t(struct ncli_cli_state *cli) {
    char tmp;
    size_t real_index = _get_line_index_from_curs(cli);
        
    if (real_index == (*cli->p_line)->len && 0 < (*cli->p_line)->len) real_index --;
    if (real_index <= 0) return;

    tmp =(*cli->p_line)->content[real_index - 1];
    (*cli->p_line)->content[real_index - 1] = (*cli->p_line)->content[real_index];
    (*cli->p_line)->content[real_index] = tmp;
    _right_arrow(cli);
}

static void _ctrl_u(struct ncli_cli_state *cli) {
    size_t real_index = _get_line_index_from_curs(cli);
    if (real_index <= 0) return;

    _ncli_delete_to_start(*cli->p_line, real_index - 1);
    cli->curs->x = (NULL != cli->prompt) ? strlen(cli->prompt) : 0;
    cli->curs->y = 0;
}

static void _ctrl_w(struct ncli_cli_state *cli) {
    int word_found = 0;
    size_t real_index = _get_line_index_from_curs(cli);
    if (real_index <= 0) return;

    /* update string and than set the cursor position using _left_arrow function */
    _ncli_delete_word(*cli->p_line, real_index);
    for (; (real_index) > 0; real_index --) {
        if (isalnum((*cli->p_line)->content[real_index]) && !word_found) word_found = 1;
        if (isspace((*cli->p_line)->content[real_index]) && word_found) break;
        _left_arrow(cli);
    }
    if (cli->curs->x > 0) _right_arrow(cli);
}

static void _clean_line(struct ncli_cli_state *cli) {
    size_t i;
    size_t prompt_len = (NULL == cli->prompt) ? 0 : strlen(cli->prompt);
    size_t used_rows = (prompt_len + (*cli->p_line)->len + cli->term_cols - 1) / cli->term_cols;

    _move_cursor_last_line(cli, 0);
    for (i = 0; i < used_rows; i ++) {
        if (write(STDOUT_FILENO, "\033[2K", 4) < 0) return;
        if (i < used_rows - 1)
            if (write(STDOUT_FILENO, "\033[A", 3) < 0) return;
    }
    if (write(STDOUT_FILENO, "\r", 1) < 0) return;
}

static void _write_line(struct ncli_cli_state *cli, const int masked) {
    size_t i;
    char mask_char = NCLI_DEFAULT_MASKED_CHAR;
    size_t prompt_len = (NULL == cli->prompt) ? 0 : strlen(cli->prompt);
    size_t used_rows = (prompt_len + (*cli->p_line)->len + cli->term_cols - 1) / cli->term_cols;
    char buf[32];
    int len;

    if (write(STDOUT_FILENO, cli->prompt, prompt_len) < 0) return;
    if (masked) {
        for (i = 0; i < (*cli->p_line)->len; i ++)
            if (write(STDOUT_FILENO, &mask_char, 1) < 0) return;
    }
    else if (write(STDOUT_FILENO, (*cli->p_line)->content, (*cli->p_line)->len) < 0) return;

    if (1 < used_rows) {
        len = snprintf(buf, sizeof buf, "\033[%ldA\r", used_rows - 1);
        if (write(STDOUT_FILENO, buf, (size_t)len) < 0) return;
        
        if (cli->curs->y > 0) {
            len = snprintf(buf, sizeof buf, "\033[%ldB", cli->curs->y);
            if (write(STDOUT_FILENO, buf, (size_t)len) < 0) return;
        }
        if (cli->curs->x > 0) {
            len = snprintf(buf, sizeof buf, "\033[%ldC", cli->curs->x);
            if (write(STDOUT_FILENO, buf, (size_t)len) < 0) return;
        }
    }
    else if (cli->curs->x > 0) {
        len = snprintf(buf, sizeof buf, "\r\033[%ldC", cli->curs->x);
        if (write(STDOUT_FILENO, buf, (size_t)len) < 0) return;
    }
}

static ncli_stat_code _handle_display(
    struct ncli_cli_state *cli,
    struct ncli_history *history,
    char *c,
    const int masked
) {
    ncli_stat_code status = NCLI_CONTINUE;
    int is_enter = (*c == NEWLINE_KEY || *c == CARR_RET_KEY);

    if (!_is_cli_state_valid(cli)) return NCLI_EXIT;
    if (!is_enter) _clean_line(cli);

    switch (*c) {
    case NEWLINE_KEY:
    case CARR_RET_KEY:
        status = NCLI_SEND_COMMAND;
        _enter(cli, history, *c);
        break;
    case BACKSPACE_KEY:
    case CTRL_H:
        _backspace(cli);
        break;
    case ESC_KEY:
        if (read(STDIN_FILENO, c, 1) <= 0) break;
        if (read(STDIN_FILENO, c, 1) <= 0) break;
        switch(*c) {
        case ARROW_UP_KEY:      _up_arrow(cli, history); break;
        case ARROW_DOWN_KEY:    _down_arrow(cli, history); break;
        case ARROW_RIGHT_KEY:   _right_arrow(cli); break;
        case ARROW_LEFT_KEY:    _left_arrow(cli); break;
        case CANC_KEY:
            if (read(STDIN_FILENO, c, 1) <= 0) break;  /* removes undesired tilde */
            _canc(cli);
            break;
        default: break;
        }
        break;
    case CTRL_A:
        cli->curs->y = 0;
        cli->curs->x = strlen(cli->prompt);
        break;
    case CTRL_B:                _left_arrow(cli); break;
    case CTRL_C:                return NCLI_EXIT;
    case CTRL_D:                _canc(cli); break;
    case CTRL_E:
        cli->curs->y = ((*cli->p_line)->len + strlen(cli->prompt)) / cli->term_cols;
        cli->curs->x = ((*cli->p_line)->len + strlen(cli->prompt)) % cli->term_cols;
        break;
    case CTRL_F:                _right_arrow(cli); break;
    case CTRL_K:                _ctrl_k(cli); break;
    case CTRL_L:                _clear_nanocli_screen(); break;
    case CTRL_N:                _down_arrow(cli, history); break;
    case CTRL_P:                _up_arrow(cli, history); break;
    case CTRL_T:                _ctrl_t(cli); break;
    case CTRL_U:                _ctrl_u(cli); break;
    case CTRL_W:                _ctrl_w(cli); break;
    default:                    _literal(cli, c); break;
    }

    if (!is_enter) _write_line(cli, masked);
    return status;
}

static ncli_stat_code _handle_char_input(struct ncli_cli_state *cli, struct ncli_history *history, const int masked) {
    fd_set readfds;
    ncli_stat_code retval = NCLI_CONTINUE;
    int ret;
    char c;

    if (NULL == cli || NULL == cli->p_line || NULL == *(cli->p_line)) return NCLI_EXIT;

    /* print prompt in the first input line */
    if (0 == (*cli->p_line)->len && NULL != cli->prompt) {
        if (write(STDOUT_FILENO, "\r", 1) <= 0) return NCLI_EXIT;
        if (write(STDOUT_FILENO, cli->prompt, strlen(cli->prompt)) < 0) return NCLI_EXIT;
        cli->curs->x = strlen(cli->prompt);
    }

    FD_ZERO(&readfds);
    FD_SET((int)STDIN_FILENO, &readfds);
    ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);

    if (-1 == ret) {
        if (EINTR == errno) return NCLI_EXIT;
    }
    else if (read(STDIN_FILENO, &c, 1) > 0) retval = _handle_display(cli, history, &c, masked);

    return retval;
}

char *_get_line(const char *prompt, const size_t max_len, struct ncli_history *history, const int masked) {
    struct ncli_cli_state *cli = _create_ncli_cli_state(prompt, max_len);
    char *response = NULL;
    ncli_stat_code code;
    if (NULL == cli) return NULL;

    if (!raw_modncli_on) {
        _enable_raw_mode();
        atexit(_restore_terminal_mode);
    }
    if (write(STDOUT_FILENO, prompt, strlen(prompt)) < 0) goto exit;
    
    do code = _handle_char_input(cli, history, masked);
    while (NCLI_SEND_COMMAND != code && NCLI_EXIT != code);
    if (NCLI_EXIT == code) goto exit;

    response = malloc((*cli->p_line)->len + 1);  /* including NULL terminator */
    strncpy(response, (*cli->p_line)->content, (*cli->p_line)->len);
    response[(*cli->p_line)->len] = '\0';
    
exit:
    _restore_terminal_mode();
    _ncli_free_cli_state(cli);
    return response;
}

char *nanocli_ask(const char *question, const size_t max_len, const int masked) {
    return _get_line(question, max_len, NULL, masked);
}

char *nanocli(const char *prompt, size_t max_str_len) {
    char *response = NULL;
    if (NULL == glob_history) glob_history = _ncli_create_history(NCLI_DEFAULT_HISTORY_MAX_SIZE);

    response = _get_line(prompt, max_str_len, glob_history, 0);
    if (NULL == response) _ncli_free_history(&glob_history);
    return response;
}

void nanocli_echo(const char *str) {
    if (NULL == str) return;
    if (write(STDOUT_FILENO, str, strlen(str)) < 0) return;
    if (write(STDOUT_FILENO, "\n", 1) < 0) return;
}
