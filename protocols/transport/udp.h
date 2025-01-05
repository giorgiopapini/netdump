#ifndef UDP_H
#define UDP_H

#include <stdint.h>

#define UDP_HEADER_LABEL        "UDP Header"
#define SRC_PORT_LABEL          "Source Port"
#define DEST_PORT_LABEL         "Dest. Port"
#define UDP_LENGTH_LABEL        "Length"
#define CHECKSUM_LABEL          "Checksum"

#define SRC_PORT(pkt)           (*((uint16_t *)(pkt)))          
#define DEST_PORT(pkt)          (*((uint16_t *)(pkt + 2)))
#define UDP_LENGTH(pkt)         (*((uint16_t *)(pkt + 4)))
#define CHECKSUM(pkt)           (*((uint16_t *)(pkt + 6)))

#define UDP_LEN                 8


size_t udp_hdr_len(const uint8_t *pkt);
void print_udp_hdr(const uint8_t *pkt);
void visualize_udp_hdr(const uint8_t *pkt);

#endif