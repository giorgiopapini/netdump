#ifndef IP_H
#define IP_h

#include <stdint.h>
#include "../../protocol_handler.h"

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
     
#define IP_HLEN(pkt)            (*((uint8_t *)(pkt)) & 0x0f)
#define IP_VERSION(pkt)         (*((uint8_t *)(pkt)) >> 4)
#define IP_TOS(pkt)             (*((uint8_t *)(pkt + 1)))
#define IP_TOTLEN(pkt)          (*((uint16_t *)(pkt + 2)))     
#define IP_ID(pkt)              (*((uint16_t *)(pkt + 4)))
#define IP_OFFSET(pkt)          (*((uint16_t *)(pkt + 6)))
#define IP_MF                   0x2000  /* 14th bit */
#define IP_DF                   0x4000  /* 15th bit */
#define IP_RF                   0x8000  /* 16th bit */
#define IP_OFFSET_MASK          0x1fff  /* last 13 bits reserved for the offset field */
#define IP_TTL(pkt)             (*((uint8_t *)(pkt + 8)))
#define IP_PROTOCOL(pkt)        (*((uint8_t *)(pkt + 9)))
#define IP_CHECKSUM(pkt)        (*((uint16_t *)(pkt + 10)))
#define IP_SRC_ADDR(pkt)        (*((uint32_t *)(pkt + 12)))
#define IP_DEST_ADDR(pkt)       (*((uint32_t *)(pkt + 16)))


protocol_info dissect_ip(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif