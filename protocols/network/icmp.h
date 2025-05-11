#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define ICMP_REPLY              0
#define ICMP_REQUEST            8

#define ICMP_HEADER_LABEL       "ICMP Header"
#define ICMP_TYPE_LABEL         "Type"
#define ICMP_CODE_LABEL         "Code"
#define ICMP_CHECKSUM_LABEL     "Checksum"
#define ICMP_ID_LABEL           "ID"
#define ICMP_SEQUENCE_LABEL     "Sequence"

#define ICMP_TYPE(pkt)          (pkt[0])
#define ICMP_CODE(pkt)          (pkt[1])
#define ICMP_CHECKSUM(pkt)      (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define ICMP_ID(pkt)            (((uint16_t)pkt[4] << 8) | (uint16_t)pkt[5])
#define ICMP_SEQUENCE(pkt)      (((uint16_t)pkt[6] << 8) | (uint16_t)pkt[7])

#define ICMP_HDR_LEN            8


protocol_info dissect_icmp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

/* use command "ping 8.8.8.8" to launch icmp packets in the local network. Than scan using netdump analize -filter "icmp" */

#endif