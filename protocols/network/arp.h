#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define ARP_REQUEST             1
#define ARP_RESPONSE            2
#define RARP_REQUEST            3
#define RARP_RESPONSE           4

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

#define HW_TYPE(pkt)            (pkt[0] << 8 | pkt[1])
#define P_TYPE(pkt)             (pkt[2] << 8 | pkt[3])
#define HW_LEN(pkt)             (pkt[4])
#define P_LEN(pkt)              (pkt[5])
#define OPERATION(pkt)          (pkt[6] << 8 | pkt[7])
#define SENDER_HW_ADDR(pkt)     ((const uint8_t *)(pkt + 8))  /* 6 bytes */
#define SENDER_P_ADDR(pkt)      ((pkt[14] << 24) | (pkt[15] << 16) | (pkt[16] << 8) | pkt[17])
#define TARGET_HW_ADDR(pkt)     ((const uint8_t *)(pkt + 18))  /* 6 bytes */
#define TARGET_P_ADDR(pkt)      ((pkt[24] << 24) | (pkt[25] << 16) | (pkt[26] << 8) | pkt[27])


protocol_info dissect_arp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

/* 
    Currently this arp description only supports ethernet technology and ip technology (no ATM or infiniband, which both have 
    different address sizes) 
*/

#endif