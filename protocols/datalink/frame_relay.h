#ifndef FRAME_RELAY_H
#define FRAME_RELAY_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define FRELAY_HEADER_LABEL     "Frame Relay Header"
#define FRELAY_DE_LABEL         "DE"
#define FRELAY_CR_LABEL         "C/R"
#define FRELAY_FECN_LABEL       "FECN"
#define FRELAY_BECN_LABEL       "BECN"
#define FRELAY_DLCI_LABEL       "DLCI"
#define FRELAY_NLPID_LABEL      "NLPID"
#define FRELAY_ETHERTYPE_LABEL  "EtherType"

#define FRELAY_DE(pkt)          ((pkt[1] & 0x08) >> 3)  /* discard eligibility */
#define FRELAY_CR(pkt)          ((pkt[1] & 0x20) >> 5)  /* command/response bit */
#define FRELAY_FECN(pkt)        ((pkt[0] & 0x02) >> 1)
#define FRELAY_BECN(pkt)        (pkt[0] & 0x01)
#define FRELAY_PROTO(pkt, len)  ((uint16_t)(((uint16_t)(pkt)[len] << 8) | (uint16_t)(pkt)[len + 1]))
#define NLPID_THRESHOLD         0xfe


protocol_info dissect_frelay(const uint8_t *pkt, size_t pkt_len);

#endif