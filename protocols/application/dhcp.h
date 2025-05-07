#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define DHCP_HEADER_LABEL       "DHCP Header"
#define DHCP_OP_LABEL           "Operation"
#define DHCP_HTYPE_LABEL        "Htype"
#define DHCP_HLEN_LABEL         "Hlen"
#define DHCP_HOPS_LABEL         "Hops"
#define DHCP_XID_LABEL          "xid"
#define DHCP_SECS_LABEL         "Elapsed Secs"
#define DHCP_BOOTP_FLAGS_LABEL  "BOOTP Flags"
#define DHCP_CIADDR_LABEL       "Client IP Address"
#define DHCP_YIADDR_LABEL       "Your (client) IP Address"
#define DHCP_SIADDR_LABEL       "Server IP Address"
#define DHCP_GIADDR_LABEL       "Relay IP Address"
#define DHCP_CHADDR_LABEL       "Client MAC Address"
#define DHCP_SNAME_LABEL        "Server Hostname"
#define DHCP_BOOTFILE_LABEL     "Boot File Name"
#define DHCP_MAGIC_COOKIE_LABEL "Magic Cookie" 

#define DHCP_OP(pkt)            (pkt[0])
#define DHCP_HTYPE(pkt)         (pkt[1])
#define DHCP_HLEN(pkt)          (pkt[2])
#define DHCP_HOPS(pkt)          (pkt[3])
#define DHCP_XID(pkt)           (((uint32_t)pkt[4] << 24) | ((uint32_t)pkt[5] << 16) | ((uint32_t)pkt[6] << 8) | (uint32_t)pkt[7])
#define DHCP_SECS(pkt)          (((uint16_t)pkt[8] << 8) | (uint16_t)pkt[9])
#define DHCP_FLAGS(pkt)         (((uint16_t)pkt[10] << 8) | (uint16_t)pkt[11])
#define DHCP_BROADCAST_MASK     0x8000
#define DHCP_CIADDR(pkt)        (((uint32_t)pkt[12] << 24) | ((uint32_t)pkt[13] << 16) | ((uint32_t)pkt[14] << 8) | (uint32_t)pkt[15])
#define DHCP_YIADDR(pkt)        (((uint32_t)pkt[16] << 24) | ((uint32_t)pkt[17] << 16) | ((uint32_t)pkt[18] << 8) | (uint32_t)pkt[19])
#define DHCP_SIADDR(pkt)        (((uint32_t)pkt[20] << 24) | ((uint32_t)pkt[21] << 16) | ((uint32_t)pkt[22] << 8) | (uint32_t)pkt[23])
#define DHCP_GIADDR(pkt)        (((uint32_t)pkt[24] << 24) | ((uint32_t)pkt[25] << 16) | ((uint32_t)pkt[26] << 8) | (uint32_t)pkt[27])
#define DHCP_CHADDR(pkt)        ((const uint8_t *)(pkt + 28))  /* After this there are NULL bytes for padding */
#define DHCP_SNAME(pkt)         ((const uint8_t *)(pkt + 44))
#define DHCP_BOOTFILE(pkt)      ((const uint8_t *)(pkt) + 108)
#define DHCP_MAGIC_COOKIE(pkt)  (((uint32_t)pkt[236] << 24) | ((uint32_t)pkt[237] << 16) | ((uint32_t)pkt[238] << 8) | (uint32_t)pkt[239])
#define DHCP_OPTIONS_OFFSET     240
#define DHCP_END_OPTIONS        0xff


protocol_info dissect_dhcp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif