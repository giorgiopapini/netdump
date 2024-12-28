#ifndef ETHER_H
#define ETHER_H

#include <stdint.h>

#define ETHER_HEADER_LABEL      "Ether Header"
#define DEST_ADDR_LABEL         "Destination MAC Address"
#define SRC_ADDR_LABEL          "Source MAC Address"
#define ETHERTYPE_LABEL         "EtherType"

#define DEST_ADDR(pkt)          (pkt)
#define SRC_ADDR(pkt)           (pkt + 6)
#define ETHERTYPE(pkt)          (*((uint16_t *)(pkt + 12)))

#define ETHER_LEN               14


size_t ether_hdr_len(const uint8_t *pkt);
void print_ether_hdr(const uint8_t *pkt);
void visualize_ether_hdr(const uint8_t *pkt);

/* 
    const uint8_t *pkt is not used in ether_hdr_len() function. It needs to be described anyways because the function pointer
    needs to be stored inside of a protocol_info table and that structure is described in such a way that it accepts only functions
    defined in this way
*/

#endif