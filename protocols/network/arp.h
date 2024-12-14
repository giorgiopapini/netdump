#ifndef ARP_H
#define ARP_H

#include <stdint.h>

void print_arp_hdr(const uint8_t *pkt);
void visualize_arp_hdr(const uint8_t *pkt);

#endif