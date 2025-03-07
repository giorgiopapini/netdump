#ifndef IPV6_H
#define IPV6_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define IPV6_HEADER_LABEL               "IPv6 Header"
#define IPV6_VERSION_LABEL              "Version"
#define IPV6_TRAFFIC_CLASS_LABEL        "Traffic Class"
#define IPV6_FLOW_LABEL                 "Flow Label"
#define IPV6_PAYLOAD_LENGTH_LABEL       "Payload Length"
#define IPV6_NEXT_HEADER_LABEL          "Next Header"
#define IPV6_HOP_LIMIT_LABEL            "Hop Limit"
#define IPV6_SRC_ADDR_LABEL             "Source Address"
#define IPV6_DEST_ADDR_LABEL            "Destination Address"

#define IPV6_VTC_FLOW(pkt)              (*((uint32_t *)(pkt)))  /* stores version, traffic_class and flow_label fields */
#define IPV6_VERSION                    0xf  /* pkt should be alredy converted using ntohl and shifted by 28 bits */
#define IPV6_TRAFFIC_CLASS              0xff  /* pkt should be alredy converted using ntohl and shifted by 20 bits */
#define IPV6_FLOW                       0xfffff  /* pkt should be alredy converted using ntohl and shifted by 0 bits */
#define IPV6_PAYLOAD_LENGTH(pkt)        (*((uint16_t *)(pkt + 4)))
#define IPV6_NEXT_HEADER(pkt)           (*((uint8_t *)(pkt + 6)))
#define IPV6_HOP_LIMIT(pkt)             (*((uint8_t *)(pkt + 7)))
#define IPV6_SRC_ADDR(pkt)              ((uint8_t *)(pkt + 8))
#define IPV6_DEST_ADDR(pkt)             ((uint8_t *)(pkt + 24))

#define IPV6_HEADER_LEN                 40


protocol_info dissect_ipv6(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif