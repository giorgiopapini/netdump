#include "terminal_handler.h"

#include <stdio.h>
#include <stddef.h>
#include <termios.h>  /* never include just <bits/termios-c_cc.h> (for safety reasons). Even if iwyu says otherwise */
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include "../status_handler.h"


static struct termios orig_termios;
static int termios_saved = 0;


void enable_raw_mode(void) {
    struct termios raw;

    if (-1 == tcgetattr(STDIN_FILENO, &orig_termios)) raise_error(TCGETATTR_ERROR, 1, NULL);
    termios_saved = 1;

    raw = orig_termios;
    raw.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (-1 == tcsetattr(STDIN_FILENO, TCSANOW, &raw)) raise_error(TCSETATTR_ERROR, 1, NULL);
}

void restore_terminal_mode(void) {
    if (termios_saved) tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void get_terminal_size(size_t *cols, size_t *rows) {
    struct winsize w;
    if (-1 == ioctl(STDOUT_FILENO, TIOCGWINSZ, &w)) raise_error(TERMINAL_SIZE_ERROR, 1, NULL);

    if (NULL != cols) *cols = w.ws_col;
    if (NULL != rows) *rows = w.ws_row;
}

int get_cursor_position(size_t *col, size_t *row) {
    char buf[64];
    struct timeval tv;
    fd_set fds;
    ssize_t ret;
    size_t i;
    int sel;
    struct termios saved, raw;

    if (tcgetattr(STDIN_FILENO, &saved) == -1) return -1;
    raw = saved;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) return -1;

    printf("\033[6n");
    fflush(stdout);

    i = 0;
    while (i < sizeof(buf) - 1) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        sel = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        if (sel <= 0) goto cleanup;

        ret = read(STDIN_FILENO, buf + i, 1);
        if (ret != 1) goto cleanup;

        if (buf[i] == 'R') {
            i ++;
            break;
        }
        i ++;
    }

    buf[i] = '\0';

    if (buf[0] == '\033' && buf[1] == '[') {
        if (NULL == row || NULL == col) goto cleanup;

        if (sscanf(buf + 2, "%zu;%zu", row, col) == 2) {
            tcsetattr(STDIN_FILENO, TCSANOW, &saved);
            return 0;
        }
    }

cleanup:
    tcsetattr(STDIN_FILENO, TCSANOW, &saved);
    return -1;
}