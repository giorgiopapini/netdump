#ifndef RIPNG_H
#define RIPNG_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define RIPNG_COMMAND_LABEL         "Command"
#define RIPNG_VERSION_LABEL         "Version"
#define RIPNG_RESERVED_LABEL        "Reserved"
#define RIPNG_NEXT_HOP_LABEL        "Next Hop"

/* commands (RFC 2080) */
#define RIPNG_COMMAND_REQUEST       1
#define RIPNG_COMMAND_RESPONSE      2

/* the only version defined by RFC 2080 */
#define RIPNG_VERSION_1             1

/* metric meaning */
#define RIPNG_METRIC_MAX            15      /* largest reachable metric */
#define RIPNG_INFINITY              16      /* route is unreachable */
#define RIPNG_NEXTHOP_METRIC        255     /* special next-hop RTE marker */

/* header (4 bytes) */
#define RIPNG_COMMAND(pkt)          ((pkt)[0])
#define RIPNG_VERSION(pkt)          ((pkt)[1])
#define RIPNG_RESERVED(pkt)         (((uint16_t)(pkt)[2] << 8) | (uint16_t)(pkt)[3])

/* route table entry (20 bytes), `e` is a pointer to the entry start */
#define RIPNG_RTE_PREFIX(e)         ((const uint8_t *)((e) + 0))   /* 16 bytes */
#define RIPNG_RTE_ROUTE_TAG(e)      (((uint16_t)(e)[16] << 8) | (uint16_t)(e)[17])
#define RIPNG_RTE_PREFIX_LEN(e)     ((e)[18])
#define RIPNG_RTE_METRIC(e)         ((e)[19])

#define RIPNG_HDR_LEN               4
#define RIPNG_RTE_LEN               20

protocol_info dissect_ripng(const uint8_t *pkt, size_t pkt_len);

#endif
