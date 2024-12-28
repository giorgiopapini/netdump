#ifndef TCP_H
#define TCP_H

#include <stdint.h>

#define TCP_HEADER_LABEL        "TCP Header"
#define SRC_PORT_LABEL          "Source Port"
#define DEST_PORT_LABEL         "Dest. Port"
#define SEQUENCE_LABEL          "Sequence Number"
#define ACK_NUM_LABEL           "Acknowledgment Number"
#define DATA_OFFSET_LABEL       "HL"
#define RESERVED_LABEL          "Reserved"
#define CWR_LABEL               "CWR"
#define ECE_LABEL               "ECE"
#define URG_LABEL               "URG"
#define ACK_LABEL               "ACK"
#define PSH_LABEL               "PSH"
#define RST_LABEL               "RST"
#define SYN_LABEL               "SYN"
#define FIN_LABEL               "FIN"
#define WINDOW_SIZE_LABEL       "Window Size"
#define CHECKSUM_LABEL          "Checksum"
#define URGENT_POINTER_LABEL    "Urgent Pointer"
#define OPTIONS_LABEL           "Options"

#define SRC_PORT(pkt)           (*((uint16_t *)(pkt)))          
#define DEST_PORT(pkt)          (*((uint16_t *)(pkt + 2)))
#define SEQUENCE(pkt)           (*((uint32_t *)(pkt + 4)))
#define ACK_NUM(pkt)            (*((uint32_t *)(pkt + 8)))
#define DATA_OFFSET(pkt)        ((*((uint8_t *)(pkt + 12))) >> 4)
#define RESERVED(pkt)           ((*((uint8_t *)(pkt + 12))) & 0x0f)  
#define FLAGS(pkt)              (*((uint8_t *)(pkt + 13)))
#define CWR                     0x80
#define ECE                     0x40
#define URG                     0x20
#define ACK                     0x10
#define PSH                     0x08
#define RST                     0x04
#define SYN                     0x02
#define FIN                     0x01
#define WINDOW_SIZE(pkt)        (*((uint16_t *)(pkt + 14)))
#define CHECKSUM(pkt)           (*((uint16_t *)(pkt + 16)))
#define URGENT_POINTER(pkt)     (*((uint16_t *)(pkt + 18)))
#define OPTIONS(pkt)            (pkt + 20)  // Pointer to options

/* https://networklessons.com/cisco/ccie-routing-switching-written/tcp-header */


size_t tcp_hdr_len(const uint8_t *pkt);
void print_tcp_hdr(const uint8_t *pkt);
void visualize_tcp_hdr(const uint8_t *pkt);

#endif