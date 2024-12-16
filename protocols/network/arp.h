#ifndef ARP_H
#define ARP_H

#include <stdint.h>

void print_arp_hdr(const uint8_t *pkt);
void visualize_arp_hdr(const uint8_t *pkt);

typedef struct arp_hdr {
    uint16_t hw_type;
    uint16_t p_type;  /* use ethertypes */
    uint8_t hw_len;
    uint8_t p_len;
    uint16_t operation;
    uint8_t sender_hardware_addr[6];  /* MAC addr */ 
    uint32_t sender_protocol_addr;  /* IP addr */
    uint8_t target_hardware_addr[6]; 
    uint32_t target_protocol_addr;
} arp_hdr;

#endif