#ifndef PPP_H
#define PPP_H

#include <stdint.h>

#define PPP_HEADER_LABEL        "PPP Header"
#define ADDRESS_LABEL           "Address"
#define CONTROL_LABEL           "Control"
#define PROTOCOL_LABEL          "Protocol"

#define ADDRESS(pkt)            (*((uint8_t *)pkt))
#define CONTROL(pkt)            (*((uint8_t *)(pkt + 1)))
#define PROTOCOL(pkt)           (*((uint16_t *)(pkt + 2)))
#define ENCAP_DATA(pkt)         (ptr + 4)

#define PPP_LEN                 4


size_t ppp_hdr_len(const uint8_t *pkt);
void print_ppp_hdr(const uint8_t *pkt);
void visualize_ppp_hdr(const uint8_t *pkt);

#endif