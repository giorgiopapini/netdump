#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>
#include "../../protocol_handler.h"

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

#define DHCP_OP(pkt)            (*((uint8_t *)(pkt)))
#define DHCP_HTYPE(pkt)         (*((uint8_t *)(pkt + 1)))
#define DHCP_HLEN(pkt)          (*((uint8_t *)(pkt + 2)))
#define DHCP_HOPS(pkt)          (*((uint8_t *)(pkt + 3)))
#define DHCP_XID(pkt)           (*((uint32_t *)(pkt + 4)))
#define DHCP_SECS(pkt)          (*((uint16_t *)(pkt + 8)))
#define DHCP_FLAGS(pkt)         (*((uint16_t *)(pkt + 10)))
#define DHCP_BROADCAST_MASK     0x8000
#define DHCP_CIADDR(pkt)        (*((uint32_t *)(pkt + 12)))
#define DHCP_YIADDR(pkt)        (*((uint32_t *)(pkt + 16)))
#define DHCP_SIADDR(pkt)        (*((uint32_t *)(pkt + 20)))
#define DHCP_GIADDR(pkt)        (*((uint32_t *)(pkt + 24)))
#define DHCP_CHADDR(pkt)        ((uint8_t *)(pkt + 28))  /* After this there are NULL bytes for padding */
#define DHCP_SNAME(pkt)         ((uint8_t *)(pkt + 44))
#define DHCP_BOOTFILE(pkt)      ((uint8_t *)(pkt) + 108)
#define DHCP_MAGIC_COOKIE(pkt)  (*((uint32_t *)(pkt + 236)))
#define DHCP_OPTIONS_OFFSET     240
#define DHCP_END_OPTIONS        0xff


protocol_info dissect_dhcp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif