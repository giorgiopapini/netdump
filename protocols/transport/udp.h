#ifndef UDP_H
#define UDP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define UDP_HEADER_LABEL        "UDP Header"
#define UDP_SRC_PORT_LABEL      "Source Port"
#define UDP_DEST_PORT_LABEL     "Dest. Port"
#define UDP_LENGTH_LABEL        "Length"
#define UDP_CHECKSUM_LABEL      "Checksum"

#define UDP_SRC_PORT(pkt)       ((pkt[0] << 8) | pkt[1])
#define UDP_DEST_PORT(pkt)      ((pkt[2] << 8) | pkt[3])
#define UDP_LENGTH(pkt)         ((pkt[4] << 8) | pkt[5])
#define UDP_CHECKSUM(pkt)       ((pkt[6] << 8) | pkt[7])

#define UDP_LEN                 8


protocol_info dissect_udp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif