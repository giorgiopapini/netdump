#include <stdlib.h>

#include "clear.h"
#include "../status_handler.h"

void execute_clear(void) {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
        #elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
        system("clear");
    #else
        raise(COMMAND_NOT_SUPPORTED_ERROR, 0, CLEAN_COMMAND);
    #endif
}