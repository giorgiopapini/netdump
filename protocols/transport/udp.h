#ifndef UDP_H
#define UDP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define UDP_HEADER_LABEL        "UDP Header"
#define UDP_SRC_PORT_LABEL      "Source Port"
#define UDP_DEST_PORT_LABEL     "Dest. Port"
#define UDP_LENGTH_LABEL        "Length"
#define UDP_CHECKSUM_LABEL      "Checksum"

#define UDP_SRC_PORT(pkt)       (((uint16_t)pkt[0] << 8) | (uint16_t)pkt[1])
#define UDP_DEST_PORT(pkt)      (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define UDP_LENGTH(pkt)         (((uint16_t)pkt[4] << 8) | (uint16_t)pkt[5])
#define UDP_CHECKSUM(pkt)       (((uint16_t)pkt[6] << 8) | (uint16_t)pkt[7])

#define UDP_LEN                 8


protocol_info dissect_udp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif