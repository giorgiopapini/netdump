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
#define DHCP_XID(pkt)           (*((uint32_t *)(pkt + 4)))
#define DHCP_SECS(pkt)          ((pkt[8] << 8) | pkt[9])
#define DHCP_FLAGS(pkt)         ((pkt[10] << 8) | pkt[11])
#define DHCP_BROADCAST_MASK     0x8000
#define DHCP_CIADDR(pkt)        ((pkt[12] << 24) | (pkt[13] << 16) | (pkt[14] << 8) | pkt[15])
#define DHCP_YIADDR(pkt)        ((pkt[16] << 24) | (pkt[17] << 16) | (pkt[18] << 8) | pkt[19])
#define DHCP_SIADDR(pkt)        ((pkt[20] << 24) | (pkt[21] << 16) | (pkt[22] << 8) | pkt[23])
#define DHCP_GIADDR(pkt)        ((pkt[24] << 24) | (pkt[25] << 16) | (pkt[26] << 8) | pkt[27])
#define DHCP_CHADDR(pkt)        ((uint8_t *)(pkt + 28))  /* After this there are NULL bytes for padding */
#define DHCP_SNAME(pkt)         ((uint8_t *)(pkt + 44))
#define DHCP_BOOTFILE(pkt)      ((uint8_t *)(pkt) + 108)
#define DHCP_MAGIC_COOKIE(pkt)  ((pkt[236] << 24) | (pkt[237] << 16) | (pkt[238] << 8) | pkt[239])
#define DHCP_OPTIONS_OFFSET     240
#define DHCP_END_OPTIONS        0xff


protocol_info dissect_dhcp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif