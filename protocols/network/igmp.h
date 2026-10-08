#ifndef IGMP_H
#define IGMP_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define IGMP_TYPE_LABEL             "Type"
#define IGMP_MAX_RESP_LABEL         "Max Resp Time"
#define IGMP_CHECKSUM_LABEL         "Checksum"
#define IGMP_GROUP_ADDR_LABEL       "Group Address"

/* message types */
#define IGMP_MEMBERSHIP_QUERY       0x11
#define IGMP_V1_MEMBERSHIP_REPORT   0x12
#define IGMP_V2_MEMBERSHIP_REPORT   0x16
#define IGMP_LEAVE_GROUP            0x17
#define IGMP_V3_MEMBERSHIP_REPORT   0x22

/* IGMPv3 membership report group record types */
#define IGMP_V3_RECORD_MODE_IS_INCLUDE       0x01
#define IGMP_V3_RECORD_MODE_IS_EXCLUDE       0x02
#define IGMP_V3_RECORD_CHANGE_TO_INCLUDE     0x03
#define IGMP_V3_RECORD_CHANGE_TO_EXCLUDE     0x04
#define IGMP_V3_RECORD_ALLOW_NEW_SOURCES     0x05
#define IGMP_V3_RECORD_BLOCK_OLD_SOURCES     0x06

/* base header (all IGMP versions) */
#define IGMP_TYPE(pkt)              ((pkt)[0])
#define IGMP_MAX_RESP(pkt)          ((pkt)[1])
#define IGMP_CHECKSUM(pkt)          (((uint16_t)(pkt)[2] << 8) | (uint16_t)(pkt)[3])
#define IGMP_GROUP_ADDR(pkt)        (((uint32_t)(pkt)[4] << 24) | ((uint32_t)(pkt)[5] << 16) | ((uint32_t)(pkt)[6] << 8) | (uint32_t)(pkt)[7])

/* IGMPv3 membership query extension (after the 8-byte base header) */
#define IGMP_V3_QUERY_S(pkt)            (((pkt)[8] & 0x08) ? 1 : 0)
#define IGMP_V3_QUERY_QRV(pkt)          ((pkt)[8] & 0x07)
#define IGMP_V3_QUERY_QQIC(pkt)         ((pkt)[9])
#define IGMP_V3_QUERY_NUM_SOURCES(pkt)  (((uint16_t)(pkt)[10] << 8) | (uint16_t)(pkt)[11])
#define IGMP_V3_QUERY_SOURCE(pkt, i)    ((((uint32_t)(pkt)[IGMP_V3_QUERY_HDR_LEN + (i) * 4] << 24) | ((uint32_t)(pkt)[IGMP_V3_QUERY_HDR_LEN + (i) * 4 + 1] << 16) | ((uint32_t)(pkt)[IGMP_V3_QUERY_HDR_LEN + (i) * 4 + 2] << 8) | (uint32_t)(pkt)[IGMP_V3_QUERY_HDR_LEN + (i) * 4 + 3]))

/* IGMPv3 membership report (base header then M group records) */
#define IGMP_V3_REPORT_NUM_RECORDS(pkt) (((uint16_t)(pkt)[6] << 8) | (uint16_t)(pkt)[7])
#define IGMP_V3_RECORD_TYPE(rec)        ((rec)[0])
#define IGMP_V3_RECORD_AUX_LEN(rec)     ((rec)[1])  /* in 32-bit words */
#define IGMP_V3_RECORD_NUM_SOURCES(rec) (((uint16_t)(rec)[2] << 8) | (uint16_t)(rec)[3])
#define IGMP_V3_RECORD_MCAST_ADDR(rec)  (((uint32_t)(rec)[4] << 24) | ((uint32_t)(rec)[5] << 16) | ((uint32_t)(rec)[6] << 8) | (uint32_t)(rec)[7])
#define IGMP_V3_RECORD_SOURCE(rec, i)   ((((uint32_t)(rec)[IGMP_V3_RECORD_HDR_LEN + (i) * 4] << 24) | ((uint32_t)(rec)[IGMP_V3_RECORD_HDR_LEN + (i) * 4 + 1] << 16) | ((uint32_t)(rec)[IGMP_V3_RECORD_HDR_LEN + (i) * 4 + 2] << 8) | (uint32_t)(rec)[IGMP_V3_RECORD_HDR_LEN + (i) * 4 + 3]))

#define IGMP_HDR_LEN                8
#define IGMP_V3_QUERY_HDR_LEN       12
#define IGMP_V3_REPORT_HDR_LEN      8
#define IGMP_V3_RECORD_HDR_LEN      8

protocol_info dissect_igmp(const uint8_t *pkt, size_t pkt_len);

#endif
