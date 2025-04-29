#ifndef HELP_H
#define HELP_H

#include "../utils/colors.h"

#define DEFAULT_SPACE           "  "  /* DEFAULT_SPACE len corresponds to PREFIX_STR len */

#define BAR                     "─"
#define PIPE                    "├"
#define CORNER                  "└"

#define DEFAULT_PIPE            PIPE BAR BAR
#define DEFAULT_CORNER          CORNER BAR BAR

void execute_help();

#endif