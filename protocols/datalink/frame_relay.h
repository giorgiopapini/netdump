#ifndef FRAME_RELAY_H
#define FRAME_RELAY_H

#include <stdint.h>
#include "../../protocol_handler.h"

#define FRELAY_HEADER_LABEL     "Frame Relay Header"
#define FRELAY_DE_LABEL         "DE"
#define FRELAY_CR_LABEL         "C/R"
#define FRELAY_FECN_LABEL       "FECN"
#define FRELAY_BECN_LABEL       "BECN"
#define FRELAY_DLCI_LABEL       "DLCI"
#define FRELAY_NLPID_LABEL      "NLPID"
#define FRELAY_ETHERTYPE_LABEL  "EtherType"


#define FRELAY_DE(pkt)          ((*((uint8_t *)(pkt + 1)) & 0x08) >> 3)  /* discard eligibility */
#define FRELAY_CR(pkt)          ((*((uint8_t *)(pkt + 1)) & 0x20) >> 5)  /* command/response bit */
#define FRELAY_FECN(pkt)        ((*((uint8_t *)(pkt)) & 0x02) >> 1)
#define FRELAY_BECN(pkt)        (*((uint8_t *)(pkt)) & 0x01)
#define FRELAY_PROTO(pkt, len)  (*((uint16_t *)(pkt + len)))

#define NLPID_THRESHOLD         0xfe


/* https://chatgpt.com/c/679d43c7-218c-8000-bcfd-35c05769061b */


protocol_info dissect_frelay(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif