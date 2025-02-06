#include <stdio.h>
#include <string.h>

#include "formats.h"


void print_ipv4(uint32_t addr) {
    char buff[IP_ADDR_STR_LEN];
    snprintf(buff, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(addr));
    printf("%s", buff);  
}

void print_ipv6(uint8_t addr[16], char output[IPV6_ADDR_STR_LEN]) {
    int i;
    int empty = 0;
    char temp[10];
    char res[IPV6_ADDR_STR_LEN] = "\0";

    for (i = 0; i < 16; i += 2) {
        if (addr[i] + addr[i + 1] != 0) {
            if (addr[i] <= 0x0f) {  /* if addr[i] has only one cypher */
                if (addr[i + 1] <= 0x0f) snprintf(temp, sizeof(temp), "%0x", addr[i + 1]);
                else snprintf(temp, sizeof(temp), "%01x%02x", addr[i], addr[i + 1]);
            }
            else snprintf(temp, sizeof(temp), "%02x%02x", addr[i], addr[i + 1]);
            empty = 0;
            strcat(res, temp);
        }
        else empty ++;

        if (i < 14 && empty < 2) strcat(res, ":");
    }


    if (NULL != output) strncpy(output, res, IPV6_ADDR_STR_LEN - 1);
    else printf("%s", res);
}

void print_mac(uint8_t mac_addr[6]) {
    int i;
    for (i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", mac_addr[i] & 0xff);
	}
    printf("%02x", mac_addr[5]);
}