#ifndef HELP_H
#define HELP_H

#include "../utils/colors.h"

#define DEFAULT_SPACE           "  "  /* DEFAULT_SPACE len corresponds to PREFIX_STR len */

#define BAR                     "─"
#define PIPE                    "├"
#define CORNER                  "└"

#define DEFAULT_PIPE            PIPE BAR BAR
#define DEFAULT_CORNER          CORNER BAR BAR


void print_cmd(const char *cmd, const char *desc);
void print_arg(const char *arg_value, const char *desc, const char *prefix_space, const char *prefix_str, const char *example);
void analize_help(void);
void devlist_help(void);
void dissectors_help(void);
void reset_help(void);
void print_help(void);
void clear_help(void);
void exit_help(void);
void save_help(void);
void execute_help(void);

#endif