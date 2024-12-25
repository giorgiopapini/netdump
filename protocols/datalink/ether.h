#ifndef ETHER_H
#define ETHER_H

#include <stdint.h>

#define ETHER_HEADER_LABEL      "Ether Header"
#define DEST_ADDR_LABEL         "Destination MAC Address"
#define SRC_ADDR_LABEL          "Source MAC Address"
#define ETHERTYPE_LABEL         "EtherType"

#define DEST_ADDR(pkt)          (pkt)
#define SRC_ADDR(pkt)           (pkt + 6)
#define ETHERTYPE(pkt)          *((uint16_t *)(pkt + 12))

#define ETHER_LEN               14               

typedef struct ether_hdr {
    uint8_t dest_addr[6];           // MAC addresses are long 48 bits; An array of 6 uint8 is needed
    uint8_t src_addr[6];            // 48 / sizeof(uint8) = 48 / 8 = 6 (octects)
    uint16_t ethertype;
} ether_hdr;

void print_ether_hdr(const uint8_t *pkt);
void visualize_ether_hdr(const uint8_t *pkt);

#endif