#ifndef HELP_H
#define HELP_H

#include "../command_handler.h"
#include "../utils/colors.h"

#define PREFIX_STR              "* "
#define DEFAULT_SPACE           "  "  /* DEFAULT_SPACE len corresponds to PREFIX_STR len */

#define BAR                     "─"
#define PIPE                    "├"
#define CORNER                  "└"

#define DEFAULT_PIPE            PIPE BAR BAR
#define DEFAULT_CORNER          CORNER BAR BAR

#define USAGE_EXAMPLE(argument, value)   (printf(YELLOW "(-%s %s)" RESET_COLOR, argument, value))

void execute_help(command *cmd);

#endif