#include "clear.h"

#include <stdlib.h>
#include <unistd.h>


void execute_clear(void) {
    if (write(STDOUT_FILENO,"\x1b[H\x1b[2J",7) <= 0) return;
}