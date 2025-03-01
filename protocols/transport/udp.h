#ifndef UDP_H
#define UDP_H

#include <stdint.h>
#include "../../protocol_handler.h"

#define UDP_HEADER_LABEL        "UDP Header"
#define UDP_SRC_PORT_LABEL      "Source Port"
#define UDP_DEST_PORT_LABEL     "Dest. Port"
#define UDP_LENGTH_LABEL        "Length"
#define UDP_CHECKSUM_LABEL      "Checksum"

#define UDP_SRC_PORT(pkt)       (*((uint16_t *)(pkt)))          
#define UDP_DEST_PORT(pkt)      (*((uint16_t *)(pkt + 2)))
#define UDP_LENGTH(pkt)         (*((uint16_t *)(pkt + 4)))
#define UDP_CHECKSUM(pkt)       (*((uint16_t *)(pkt + 6)))

#define UDP_LEN                 8


protocol_info dissect_udp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif