#ifndef ETHER_H
#define ETHER_H

#include <stdint.h>

struct ether_header {
    uint8_t dest_addr[6];           // MAC addresses are long 48 bits; An array of 6 uint8 is needed
    uint8_t src_addr[6];            // 48 / sizeof(uint8) = 48 / 8 = 6 (octects)
    uint16_t ethertype;
} ether_header;

#endif