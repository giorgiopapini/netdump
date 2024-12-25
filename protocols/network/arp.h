#ifndef ARP_H
#define ARP_H

#include <stdint.h>

#define ARP_REQUEST             1
#define ARP_RESPONSE            2
#define RARP_REQUEST            3
#define RARP_RESPONSE           4

#define ARP_REQUEST_STR         "who-has"
#define ARP_RESPONSE_STR        "reply"
#define RARP_REQUEST_STR        ""
#define RARP_RESPONSE_STR       ""

#define ARP_HEADER_LABEL        "ARP Header"
#define HW_TYPE_LABEL           "HTYPE"
#define P_TYPE_LABEL            "PTYPE"
#define HW_LEN_LABEL            "HLEN"
#define P_LEN_LABEL             "PLEN"
#define OPERATION_LABEL         "Operation"
#define SENDER_HW_ADDR_LABEL    "Sender MAC Address"
#define SENDER_P_ADDR_LABEL     "Sender IP Address"
#define TARGET_HW_ADDR_LABEL    "Target MAC Address"
#define TARGET_P_ADDR_LABEL     "Target IP Address"

#define HW_TYPE(pkt)            *((uint16_t *)(pkt))
#define P_TYPE(pkt)             *((uint16_t *)(pkt + 2))
#define HW_LEN(pkt)             *((uint8_t *)(pkt + 4))
#define P_LEN(pkt)              *((uint8_t *)(pkt + 5))
#define OPERATION(pkt)          *((uint16_t *)(pkt + 6))
#define SENDER_HW_ADDR(pkt)     (pkt + 8)  /* 6 bytes */
#define SENDER_P_ADDR(pkt)      *((uint32_t *)(pkt + 14))
#define TARGET_HW_ADDR(pkt)     (pkt + 18)  /* 6 bytes */
#define TARGET_P_ADDR(pkt)      *((uint32_t *)(pkt + 24))

#define ARP_LEN                 28


void print_arp_hdr(const uint8_t *pkt);
void visualize_arp_hdr(const uint8_t *pkt);

#endif