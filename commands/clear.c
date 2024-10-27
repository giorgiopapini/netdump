#include <stdlib.h>
#include "clear.h"

void execute_clear(command *cmd) {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #elif defined(__linux__) || defined(__APPLE__)
        system("clear");
    #else
        raise(COMMAND_NOT_SUPPORTED_ERROR, 0, CLEAN_COMMAND);
    #endif
}