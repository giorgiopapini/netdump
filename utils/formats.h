#ifndef FORMATS_H
#define FORMATS_H

#include "../command_handler.h"

#define MAC_TO_STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_ADDR_FORMAT "%02x:%02x:%02x:%02x:%02x:%02x"

#define IP_TO_STR(a) (a)[0], (a)[1], (a)[2], (a)[3]
#define IP_ADDR_FORMAT "%d.%d.%d.%d"
#define IP_ADDR_STR_LEN 16  /* 255.255.255.255'\0' */

#define UNKNOWN "unknown"  /* it should NEVER be NULL */

#define COMMAND_FORMAT "<command> " ARG_PREFIX "<arg> <value>"
#define STRING_DELIMITER '"'

#endif