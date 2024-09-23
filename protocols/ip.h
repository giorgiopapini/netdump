#ifndef IP_H
#define IP_h

#include <stdint.h>

struct ip_header {
    uint8_t vhlen;
    uint8_t tos;
    uint16_t totlen;
    uint16_t identification;
    uint16_t offset_field;
    #define MF      0x2000  /* 14th bit */
    #define DF      0x4000  /* 15th bit */
    #define OFFSET_MASK     0x1fff  /* last 13 bits reserved for the offset field */
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_addr;
    uint32_t dest_addr;
} ip_header;

#define IP_HLEN(ip_header)      (((ip_header)->vhlen) & 0x0f)
#define IP_VERSION(ip_header)   (((ip_header)->vhlen) >> 4)

#endif