#ifndef ICMPV6_H
#define ICMPV6_H

#include <stddef.h>
#include <stdint.h>

#include "../../utils/protocol.h"

#define ICMPV6_HEADER_LABEL         "ICMPv6 Header"
#define ICMPV6_TYPE_LABEL           "Type"
#define ICMPV6_CODE_LABEL           "Code"
#define ICMPV6_CHECKSUM_LABEL       "Checksum"
#define ICMPV6_ID_LABEL             "ID"
#define ICMPV6_SEQ_LABEL            "Sequence"

#define ICMPV6_TYPE(pkt)            (pkt[0])
#define ICMPV6_CODE(pkt)            (pkt[1])
#define ICMPV6_CHECKSUM(pkt)        (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define ICMPV6_ID(pkt)              (((uint16_t)pkt[4] << 8) | (uint16_t)pkt[5])
#define ICMPV6_SEQ(pkt)             (((uint16_t)pkt[6] << 8) | (uint16_t)pkt[7])

#define ICMPV6_DEST_UNREACH_CODE    1
#define ICMPV6_TOO_LARGE_CODE       2
#define ICMPV6_TIME_EXCEED_CODE     3
#define ICMPV6_PARAM_ERR_CODE       4

#define ICMPV6_ECHO_REQUEST_CODE    128
#define ICMPV6_ECHO_REPLY_CODE      129

#define ICMPV6_MULTI_QUERY_CODE     130
#define ICMPV6_MULTI_REPORT_CODE    131
#define ICMPV6_MULTI_DONE_CODE      132
#define ICMPV6_ROUTER_REQUEST_CODE  133
#define ICMPV6_ROUTER_ANNOUNCE_CODE 134
#define ICMPV6_ADJ_ROUTER_REQ_CODE  135
#define ICMPV6_ADJ_ROUTER_ANN_CODE  136
#define ICMPV6_MESSAGE_REDIR_CODE   137

#define ICMPV6_HDR_LEN              8


protocol_info dissect_icmpv6(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif