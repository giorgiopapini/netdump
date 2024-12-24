#ifndef FORMATS_H
#define FORMATS_H

#include <stdint.h>

#include "../command_handler.h"

#define MAC_TO_STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_ADDR_FORMAT "%02x:%02x:%02x:%02x:%02x:%02x"

#define IP_TO_STR(a) (a)[0], (a)[1], (a)[2], (a)[3]
#define IP_ADDR_FORMAT "%d.%d.%d.%d"
#define IP_ADDR_STR_LEN 16  /* 255.255.255.255'\0' */

#define UNKNOWN "unknown"  /* it should NEVER be NULL */
#define PREFIX_STR "* "

void ipv4_to_str(char *dest, uint32_t addr);
void print_ipv4(uint32_t addr);
void print_mac(uint8_t mac_addr[6]);

#endif