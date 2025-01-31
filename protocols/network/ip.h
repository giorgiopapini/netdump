#ifndef IP_H
#define IP_h

#include <stdint.h>
#include "../../protocol_handler.h"

#define IP_HEADER_LABEL     "IP Header"
#define VERSION_LABEL       "Version"
#define IHL_LABEL           "IHL"
#define TOS_LABEL           "Type of Service"
#define TOTLEN_LABEL        "Total Length"
#define ID_LABEL            "Identification"
#define MF_LABEL            "MF"
#define DF_LABEL            "DF"
#define RF_LABEL            "RF"
#define OFFSET_LABEL        "Fragment Offset"
#define TTL_LABEL           "Time to Live"
#define PROTOCOL_LABEL      "Protocol"
#define CHECKSUM_LABEL      "Checksum"
#define SRC_ADDR_LABEL      "Source Address"
#define DEST_ADDR_LABEL     "Destination Address"
     
#define IP_HLEN(pkt)        (*((uint8_t *)(pkt)) & 0x0f)
#define IP_VERSION(pkt)     (*((uint8_t *)(pkt)) >> 4)
#define TOS(pkt)            (*((uint8_t *)(pkt + 1)))
#define TOTLEN(pkt)         (*((uint16_t *)(pkt + 2)))     
#define ID(pkt)             (*((uint16_t *)(pkt + 4)))
#define OFFSET(pkt)         (*((uint16_t *)(pkt + 6)))
#define MF                  0x2000  /* 14th bit */
#define DF                  0x4000  /* 15th bit */
#define RF                  0x8000  /* 16th bit */
#define OFFSET_MASK         0x1fff  /* last 13 bits reserved for the offset field */
#define TTL(pkt)            (*((uint8_t *)(pkt + 8)))
#define PROTOCOL(pkt)       (*((uint8_t *)(pkt + 9)))
#define CHECKSUM(pkt)       (*((uint16_t *)(pkt + 10)))
#define SRC_ADDR(pkt)       (*((uint32_t *)(pkt + 12)))
#define DEST_ADDR(pkt)      (*((uint32_t *)(pkt + 16)))


protocol_info dissect_ip(const uint8_t *pkt, const char *proto_name, output_format fmt);

#endif