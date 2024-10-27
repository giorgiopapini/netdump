#include <stdio.h>
#include "help.h"


void execute_help(command *cmd) {
    char *cmd_label = get_raw_val(cmd, CMD_ARG);
    
    if (NULL == cmd_label) printf("HELP LIST\n");
    else {
        printf("|%s|\n", cmd_label);
    }
    
}