#ifndef IP_H
#define IP_h

#include <stdint.h>
#include "../../utils/protocol.h"

#define IP_HEADER_LABEL         "IP Header"
#define IP_VERSION_LABEL        "Version"
#define IP_IHL_LABEL            "IHL"
#define IP_TOS_LABEL            "Type of Service"
#define IP_TOTLEN_LABEL         "Total Length"
#define IP_ID_LABEL             "Identification"
#define IP_MF_LABEL             "MF"
#define IP_DF_LABEL             "DF"
#define IP_RF_LABEL             "RF"
#define IP_OFFSET_LABEL         "Fragment Offset"
#define IP_TTL_LABEL            "Time to Live"
#define IP_PROTOCOL_LABEL       "Protocol"
#define IP_CHECKSUM_LABEL       "Checksum"
#define IP_SRC_ADDR_LABEL       "Source Address"
#define IP_DEST_ADDR_LABEL      "Destination Address"
    
#define NP_IP_HLEN(pkt)         (pkt[0] & 0x0f)
#define NP_IP_VERSION(pkt)      (pkt[0] >> 4)
#define NP_IP_TOS(pkt)          (pkt[1])
#define NP_IP_TOTLEN(pkt)       ((uint16_t)(pkt[2] << 8) | (uint16_t)pkt[3])
#define NP_IP_ID(pkt)           ((uint16_t)(pkt[4] << 8) | (uint16_t)pkt[5])
#define NP_IP_OFFSET(pkt)       ((uint16_t)(pkt[6] << 8) | (uint16_t)pkt[7])
#define NP_IP_MF                0x2000  /* 14th bit */
#define NP_IP_DF                0x4000  /* 15th bit */
#define NP_IP_RF                0x8000  /* 16th bit */
#define NP_IP_OFFSET_MASK       0x1fff  /* last 13 bits reserved for the offset field */
#define NP_IP_TTL(pkt)          (pkt[8])
#define NP_IP_PROTOCOL(pkt)     (pkt[9])
#define NP_IP_CHECKSUM(pkt)     ((uint16_t)(pkt[10] << 8) | (uint16_t)pkt[11])
#define NP_IP_SRC_ADDR(pkt)     ((uint32_t)(pkt[12] << 24) | (uint32_t)(pkt[13] << 16) | (uint32_t)(pkt[14] << 8) | (uint32_t)pkt[15])
#define NP_IP_DEST_ADDR(pkt)    ((uint32_t)(pkt[16] << 24) | (uint32_t)(pkt[17] << 16) | (uint32_t)(pkt[18] << 8) | (uint32_t)pkt[19])


protocol_info dissect_ip(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif