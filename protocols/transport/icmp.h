#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>

#define ICMP_REPLY              0
#define ICMP_REQUEST            8

#define ICMP_HEADER_LABEL       "ICMP Header"
#define TYPE_LABEL              "Type"
#define CODE_LABEL              "Code"
#define CHECKSUM_LABEL          "Checksum"
#define ID_LABEL                "ID"
#define SEQUENCE_LABEL          "Sequence"

#define TYPE(pkt)               (*((uint8_t *)pkt))
#define CODE(pkt)               (*((uint8_t *)(pkt + 1)))
#define CHECKSUM(pkt)           (*((uint16_t *)(pkt + 2)))
#define ID(pkt)                 (*((uint16_t *)(pkt + 4)))
#define SEQUENCE(pkt)           (*((uint16_t *)(pkt + 6)))

#define ICMP_LEN                8


size_t icmp_hdr_len(const uint8_t *pkt);
void print_icmp_hdr(const uint8_t *pkt);
void visualize_icmp_hdr(const uint8_t *pkt);

/* use command "ping 8.8.8.8" to launch icmp packets in the local network. Than scan using netdump analize -filter "icmp" */

#endif