#ifndef RIP_H
#define RIP_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define RIP_COMMAND_LABEL           "Command"
#define RIP_VERSION_LABEL           "Version"
#define RIP_ROUTING_DOMAIN_LABEL    "Routing Domain"
#define RIP_AUTH_TYPE_LABEL         "Authentication Type"
#define RIP_PACKET_LEN_LABEL        "RIPv2 Packet Length"
#define RIP_KEY_ID_LABEL            "Key ID"
#define RIP_AUTH_DATA_LEN_LABEL     "Authentication Data Length"
#define RIP_SEQUENCE_LABEL          "Sequence Number"

/* commands (RFC 2453) */
#define RIP_COMMAND_REQUEST         1
#define RIP_COMMAND_RESPONSE        2

/* versions */
#define RIP_VERSION_1               1
#define RIP_VERSION_2               2

/* address family identifiers */
#define RIP_AFI_IP                  2
#define RIP_AFI_AUTHENTICATION      0xFFFF

/* authentication types (RFC 2453 / RFC 4822) */
#define RIP_AUTH_TYPE_NONE          0
#define RIP_AUTH_TYPE_SIMPLE        2
#define RIP_AUTH_TYPE_CRYPTO        3

#define RIP_AUTH_TRAILER_TAG        0x0001

/* header */
#define RIP_COMMAND(pkt)          ((pkt)[0])
#define RIP_VERSION(pkt)          ((pkt)[1])
#define RIP_ROUTING_DOMAIN(pkt)   (((uint16_t)(pkt)[2] << 8) | (uint16_t)(pkt)[3])

/* authentication entry: occupies the first entry slot when AFI == 0xFFFF */
#define RIP_AUTH_AFI(pkt)         (((uint16_t)(pkt)[4] << 8) | (uint16_t)(pkt)[5])
#define RIP_AUTH_TYPE(pkt)        (((uint16_t)(pkt)[6] << 8) | (uint16_t)(pkt)[7])
#define RIP_AUTH_PACKET_LEN(pkt)  (((uint16_t)(pkt)[8] << 8) | (uint16_t)(pkt)[9])
#define RIP_AUTH_KEY_ID(pkt)      ((pkt)[10])
#define RIP_AUTH_DATA_LEN(pkt)    ((pkt)[11])
#define RIP_AUTH_SEQUENCE(pkt)    (((uint32_t)(pkt)[12] << 24) | ((uint32_t)(pkt)[13] << 16) | ((uint32_t)(pkt)[14] << 8) | (uint32_t)(pkt)[15])
#define RIP_AUTH_RESERVED_1(pkt)  (((uint32_t)(pkt)[16] << 24) | ((uint32_t)(pkt)[17] << 16) | ((uint32_t)(pkt)[18] << 8) | (uint32_t)(pkt)[19])
#define RIP_AUTH_RESERVED_2(pkt)  (((uint32_t)(pkt)[20] << 24) | ((uint32_t)(pkt)[21] << 16) | ((uint32_t)(pkt)[22] << 8) | (uint32_t)(pkt)[23])

/* authentication trailer, anchored at the RIPv2 Packet Length offset */
#define RIP_AUTH_TRAILER_AFI(pkt, off)       (((uint16_t)(pkt)[off] << 8) | (uint16_t)(pkt)[(off) + 1])
#define RIP_AUTH_TRAILER_TAG_FIELD(pkt, off) (((uint16_t)(pkt)[(off) + 2] << 8) | (uint16_t)(pkt)[(off) + 3])

/* route entry (20 bytes), `e` is a pointer to the entry start */
#define RIP_ENTRY_AFI(e)          (((uint16_t)(e)[0] << 8) | (uint16_t)(e)[1])
#define RIP_ENTRY_TAG(e)          (((uint16_t)(e)[2] << 8) | (uint16_t)(e)[3])
#define RIP_ENTRY_ADDR(e)         (((uint32_t)(e)[4] << 24) | ((uint32_t)(e)[5] << 16) | ((uint32_t)(e)[6] << 8) | (uint32_t)(e)[7])
#define RIP_ENTRY_MASK(e)         (((uint32_t)(e)[8] << 24) | ((uint32_t)(e)[9] << 16) | ((uint32_t)(e)[10] << 8) | (uint32_t)(e)[11])
#define RIP_ENTRY_NEXTHOP(e)      (((uint32_t)(e)[12] << 24) | ((uint32_t)(e)[13] << 16) | ((uint32_t)(e)[14] << 8) | (uint32_t)(e)[15])
#define RIP_ENTRY_METRIC(e)       (((uint32_t)(e)[16] << 24) | ((uint32_t)(e)[17] << 16) | ((uint32_t)(e)[18] << 8) | (uint32_t)(e)[19])

#define RIP_HDR_LEN                 4
#define RIP_AUTH_ENTRY_LEN          20
#define RIP_HEADER_WITH_AUTH_LEN    24
#define RIP_ENTRY_LEN               20
#define RIP_AUTH_TRAILER_MARKER_LEN 4

protocol_info dissect_rip(const uint8_t *pkt, size_t pkt_len);

#endif
