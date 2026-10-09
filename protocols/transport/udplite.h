#ifndef UDPLITE_H
#define UDPLITE_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define UDPLITE_HEADER_LABEL                "UDP-Lite Header"
#define UDPLITE_SRC_PORT_LABEL              "Source Port"
#define UDPLITE_DEST_PORT_LABEL             "Dest. Port"
#define UDPLITE_CHECKSUM_COVERAGE_LABEL     "Checksum Coverage"
#define UDPLITE_CHECKSUM_LABEL              "Checksum"

#define UDPLITE_SRC_PORT(pkt)           (((uint16_t)pkt[0] << 8) | (uint16_t)pkt[1])
#define UDPLITE_DEST_PORT(pkt)          (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define UDPLITE_CHECKSUM_COVERAGE(pkt)  (((uint16_t)pkt[4] << 8) | (uint16_t)pkt[5])
#define UDPLITE_CHECKSUM(pkt)           (((uint16_t)pkt[6] << 8) | (uint16_t)pkt[7])

#define UDPLITE_HDR_LEN                 8

protocol_info dissect_udplite(const uint8_t *pkt, size_t pkt_len);

#endif
