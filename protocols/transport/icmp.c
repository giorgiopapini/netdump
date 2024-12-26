#include <stdio.h>

#include "icmp.h"

size_t icmp_hdr_len(const uint8_t *pkt) { return ICMP_LEN; }

void print_icmp_hdr(const uint8_t *pkt) {
    printf("\nDUMMY ICMP\n");
}

void visualize_icmp_hdr(const uint8_t *pkt) {
    printf("\nDUMMY ICMP\n");
}