#ifndef PPP_H
#define PPP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define PPP_HEADER_LABEL        "PPP Header"
#define ADDRESS_LABEL           "Address"
#define CONTROL_LABEL           "Control"
#define PROTOCOL_LABEL          "Protocol"

#define ADDRESS(pkt)            (pkt[0])
#define CONTROL(pkt)            (pkt[1])
#define PROTOCOL(pkt)           (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define ENCAP_DATA(pkt)         ((const uint8_t *)(pkt + 4))

#define PPP_HDR_LEN             4


protocol_info dissect_ppp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif