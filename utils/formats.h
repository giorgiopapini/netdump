#ifndef FORMATS_H
#define FORMATS_H

#include <stdint.h>

#include "../command_handler.h"

#define PROMPT_STRING "netdump > "

#define MAC_TO_STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_ADDR_FORMAT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ADDR_STR_LEN 18

#define IP_TO_STR(addr) \
    ((const uint8_t)((addr) >> 24) & 0xFF), \
    ((const uint8_t)((addr) >> 16) & 0xFF), \
    ((const uint8_t)((addr) >> 8) & 0xFF), \
    ((const uint8_t)(addr) & 0xFF)
#define IP_ADDR_FORMAT "%u.%u.%u.%u"
#define IP_ADDR_STR_LEN 16  /* 255.255.255.255'\0' */

#define IPV6_ADDR_STR_LEN 40  /* 0000:0000:0000:0000:0000:0000:0000:0000'\0' */

#define UNKNOWN                     "unknown"  /* it should NEVER be NULL */
#define PREFIX_STR                  "* "

#define DEFAULT_SPACE               "  "  /* DEFAULT_SPACE len corresponds to PREFIX_STR len */

#define BAR                         "─"
#define PIPE                        "├"
#define CORNER                      "└"

#define DEFAULT_PIPE                PIPE BAR BAR
#define DEFAULT_CORNER              CORNER BAR BAR

#define COMMAND_FORMAT              "<command> " ARG_PREFIX "<arg> <value>"
#define ARG_PREFIX                  "-"
#define ARG_STR_DELIMITER           "\""
#define STRINGS_SEPARATOR           ","

void print_ipv4(const uint32_t addr);
void print_ipv6(const uint8_t addr[16], char output[IPV6_ADDR_STR_LEN]);
void print_mac(const uint8_t mac_addr[6]);

#endif