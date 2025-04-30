#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define TCP_HEADER_LABEL            "TCP Header"
#define TCP_SRC_PORT_LABEL          "Source Port"
#define TCP_DEST_PORT_LABEL         "Dest. Port"
#define TCP_SEQUENCE_LABEL          "Sequence Number"
#define TCP_ACK_NUM_LABEL           "Acknowledgment Number"
#define TCP_DATA_OFFSET_LABEL       "HL"
#define TCP_RESERVED_LABEL          "Reserved"
#define TCP_CWR_LABEL               "CWR"
#define TCP_ECE_LABEL               "ECE"
#define TCP_URG_LABEL               "URG"
#define TCP_ACK_LABEL               "ACK"
#define TCP_PSH_LABEL               "PSH"
#define TCP_RST_LABEL               "RST"
#define TCP_SYN_LABEL               "SYN"
#define TCP_FIN_LABEL               "FIN"
#define TCP_WINDOW_SIZE_LABEL       "Window Size"
#define TCP_CHECKSUM_LABEL          "Checksum"
#define TCP_URGENT_POINTER_LABEL    "Urgent Pointer"
#define TCP_OPTIONS_LABEL           "Options"

#define TCP_SRC_PORT(pkt)           ((pkt[0] << 8) | pkt[1])
#define TCP_DEST_PORT(pkt)          ((pkt[2] << 8) | pkt[3])
#define TCP_SEQUENCE(pkt)           ((pkt[4] << 24) | (pkt[5] << 16) | (pkt[6] << 8) | pkt[7])
#define TCP_ACK_NUM(pkt)            ((pkt[8] << 24) | (pkt[9] << 16) | (pkt[10] << 8) | pkt[11])
#define TCP_DATA_OFFSET(pkt)        (pkt[12] >> 4)
#define TCP_RESERVED(pkt)           (pkt[12] & 0x0f)  
#define TCP_FLAGS(pkt)              (pkt[13])
#define TCP_CWR                     0x80
#define TCP_ECE                     0x40
#define TCP_URG                     0x20
#define TCP_ACK                     0x10
#define TCP_PSH                     0x08
#define TCP_RST                     0x04
#define TCP_SYN                     0x02
#define TCP_FIN                     0x01
#define TCP_WINDOW_SIZE(pkt)        ((pkt[14] << 8) | pkt[15])
#define TCP_CHECKSUM(pkt)           ((pkt[16] << 8) | pkt[17])
#define TCP_URGENT_POINTER(pkt)     ((pkt[18] << 8) | pkt[19])
#define TCP_OPTIONS(pkt)            ((const uint8_t *)(pkt + 20))  /* pointer to options, 20 is the fixed amount of bytes of the tcp header */

/* https://networklessons.com/cisco/ccie-routing-switching-written/tcp-header */


protocol_info dissect_tcp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif