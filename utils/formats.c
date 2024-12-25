#include <stdio.h>

#include "formats.h"


void print_ipv4(uint32_t addr) {
    char buff[IP_ADDR_STR_LEN];
    snprintf(buff, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(addr));
    printf("%s", buff);  
}

void print_mac(uint8_t mac_addr[6]) {
    int i;
    for (i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", mac_addr[i] & 0xff);
	}
    printf("%02x", mac_addr[5]);
}