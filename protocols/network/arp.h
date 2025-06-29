#ifndef ARP_H
#define ARP_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

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

#define HW_TYPE(pkt)            (((uint16_t)pkt[0] << 8) | (uint16_t)pkt[1])
#define P_TYPE(pkt)             (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])
#define HW_LEN(pkt)             (pkt[4])
#define P_LEN(pkt)              (pkt[5])
#define OPERATION(pkt)          (((uint16_t)pkt[6] << 8) | (uint16_t)pkt[7])
#define SENDER_HW_ADDR(pkt)     ((const uint8_t *)(pkt + 8))  /* 6 bytes */
#define SENDER_P_ADDR(pkt)      (((uint32_t)pkt[14] << 24) | ((uint32_t)pkt[15] << 16) | ((uint32_t)pkt[16] << 8) | (uint32_t)pkt[17])
#define TARGET_HW_ADDR(pkt)     ((const uint8_t *)(pkt + 18))  /* 6 bytes */
#define TARGET_P_ADDR(pkt)      (((uint32_t)pkt[24] << 24) | ((uint32_t)pkt[25] << 16) | ((uint32_t)pkt[26] << 8) | (uint32_t)pkt[27])

#define ARP_HDR_LEN                 28


protocol_info dissect_arp(const uint8_t *pkt, size_t pkt_len);

/* 
    Currently this arp description only supports ethernet technology and ip technology (no ATM or infiniband, which both have 
    different address sizes) 
*/

#endif