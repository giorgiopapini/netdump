#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>

#define ICMP_LEN                8


size_t icmp_hdr_len(const uint8_t *pkt);
void print_icmp_hdr(const uint8_t *pkt);
void visualize_icmp_hdr(const uint8_t *pkt);

/* use command "ping 8.8.8.8" to launch icmp packets in the local network. Than scan using netdump analize -filter "icmp" */

#endif