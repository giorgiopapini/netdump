#include "terminal_handler.h"

#include <termios.h>
#include <unistd.h>

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