#ifndef DNS_H
#define DNS_H

#include <stdint.h>
#include "../../protocol_handler.h"

#define DNS_HEADER_LABEL                "DNS Header"
#define DNS_TRANSACTION_ID_LABEL        "Transaction ID"
#define DNS_QR_LABEL                    "QR"
#define DNS_OPCODE_LABEL                "OPCODE"
#define DNS_AA_LABEL                    "AA"
#define DNS_TC_LABEL                    "TC"
#define DNS_RD_LABEL                    "RD"
#define DNS_RA_LABEL                    "RA"
#define DNS_AD_LABEL                    "AD"
#define DNS_CD_LABEL                    "CD"
#define DNS_RCODE_LABEL                 "RCODE"
#define DNS_QUESTIONS_LABEL             "Questions"
#define DNS_ANSWER_RRS_LABEL            "Answer RRs"
#define DNS_AUTH_RRS_LABEL              "Authority RRs"
#define DNS_ADDITIONAL_RRS_LABEL        "Additional RRs"

#define DNS_TRANSACTION_ID(pkt)         (*((uint16_t *)(pkt)))
#define DNS_FLAGS(pkt)                  (*((uint16_t *)(pkt + 2)))
#define DNS_QR                          0x8000              
#define DNS_OPCODE                      0x7800
#define DNS_AA                          0x0400
#define DNS_TC                          0x0200
#define DNS_RD                          0x0100
#define DNS_RA                          0x0080
#define DNS_Z                           0x0040
#define DNS_AD                          0x0020
#define DNS_CD                          0x0010
#define DNS_RCODE                       0x000f
#define DNS_QUESTIONS(pkt)              (*((uint16_t *)(pkt + 4)))
#define DNS_ANSWER_RRS(pkt)             (*((uint16_t *)(pkt + 6)))
#define DNS_AUTH_RRS(pkt)               (*((uint16_t *)(pkt + 8)))
#define DNS_ADDITIONAL_RRS(pkt)         (*((uint16_t *)(pkt + 10)))

#define DNS_HDR_LEN                     12


protocol_info dissect_dns(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif