#ifndef ETHER_H
#define ETHER_H

#include <stdint.h>

typedef struct ether_hdr {
    uint8_t dest_addr[6];           // MAC addresses are long 48 bits; An array of 6 uint8 is needed
    uint8_t src_addr[6];            // 48 / sizeof(uint8) = 48 / 8 = 6 (octects)
    uint16_t ethertype;
} ether_hdr;

void print_ether_hdr(const uint8_t *pkt);
void visualize_ether_hdr(const uint8_t *pkt);

#endif