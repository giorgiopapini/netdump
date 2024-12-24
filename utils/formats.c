#include <stdio.h>

#include "formats.h"

void ipv4_to_str(char *dest, uint32_t addr) {
    uint8_t bytes[4];
    bytes[3] = addr & 0xFF;
    bytes[2] = (addr >> 8) & 0xFF;
    bytes[1] = (addr >> 16) & 0xFF;
    bytes[0] = (addr >> 24) & 0xFF;
    snprintf(dest, 16, IP_ADDR_FORMAT, IP_TO_STR(bytes));
    dest[IP_ADDR_STR_LEN - 1] = '\0';
}

void print_ipv4(uint32_t addr) {
    char buff[IP_ADDR_STR_LEN];
    ipv4_to_str(buff, addr);
    printf("%s", buff);  
}

void print_mac(uint8_t mac_addr[6]) {
    int i;
    for (i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", mac_addr[i] & 0xff);
	}
    printf("%02x", mac_addr[5]);
}