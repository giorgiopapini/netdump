#ifndef TCP_H
#define TCP_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

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

#define TCP_SRC_PORT(pkt)           (((uint16_t)pkt[0] << 8) | (uint16_t)pkt[1])
#define TCP_DEST_PORT(pkt)          (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define TCP_SEQUENCE(pkt)           (((uint32_t)pkt[4] << 24) | ((uint32_t)pkt[5] << 16) | ((uint32_t)pkt[6] << 8) | (uint32_t)pkt[7])
#define TCP_ACK_NUM(pkt)            (((uint32_t)pkt[8] << 24) | ((uint32_t)pkt[9] << 16) | ((uint32_t)pkt[10] << 8) | (uint32_t)pkt[11])
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
#define TCP_WINDOW_SIZE(pkt)        (((uint16_t)pkt[14] << 8) | (uint16_t)pkt[15])
#define TCP_CHECKSUM(pkt)           (((uint16_t)pkt[16] << 8) | (uint16_t)pkt[17])
#define TCP_URGENT_POINTER(pkt)     (((uint16_t)pkt[18] << 8) | (uint16_t)pkt[19])
#define TCP_OPTIONS(pkt)            ((const uint8_t *)(pkt + 20))  /* pointer to options, 20 is the fixed amount of bytes of the tcp header */

#define TCP_HDR_LEN(pkt)            (TCP_DATA_OFFSET(pkt) * 4)

/* https://networklessons.com/cisco/ccie-routing-switching-written/tcp-header */


protocol_info dissect_tcp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif