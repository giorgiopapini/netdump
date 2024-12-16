#ifndef IP_H
#define IP_h

#include <stdint.h>

#define IP_HEADER_LABEL "IP Header"
#define VERSION_LABEL "Version"
#define IHL_LABEL "IHL"
#define TOS_LABEL "Type of Service"
#define TOTLEN_LABEL "Total Length"
#define ID_LABEL "Identification"
#define MF_LABEL "MF"
#define DF_LABEL "DF"
#define RF_LABEL "RF"
#define OFFSET_LABEL "Fragment Offset"
#define TTL_LABEL "Time to Live"
#define PROTOCOL_LABEL "Protocol"
#define CHECKSUM_LABEL "Checksum"
#define SRC_ADDR_LABEL "Source Address"
#define DEST_ADDR_LABEL "Destination Address"

/* https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers */

typedef struct ip_hdr {     /* IMPORTANT!! --> Find a way to manage options, not here inside of ip_hdr */ 
    uint8_t vhlen;
    uint8_t tos;
    uint16_t totlen;
    uint16_t identification;
    uint16_t offset_field;
    #define MF      0x2000  /* 14th bit */
    #define DF      0x4000  /* 15th bit */
    #define RF      0x8000  /* 16th bit */
    #define OFFSET_MASK     0x1fff  /* last 13 bits reserved for the offset field */
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_addr;
    uint32_t dest_addr;
} ip_hdr;

#define IP_HLEN(ip_hdr)      (((ip_header)->vhlen) & 0x0f)
#define IP_VERSION(ip_hdr)   (((ip_header)->vhlen) >> 4)

void print_ip_hdr(const uint8_t *pkt);
void visualize_ip_hdr(const uint8_t *pkt);

#endif