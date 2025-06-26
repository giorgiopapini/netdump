#ifndef VLAN_H
#define VLAN_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define VLAN_HEADER_LABEL       "VLAN Header"
#define VLAN_TPID_LABEL         "Tag Protocol ID"
#define VLAN_PCP_LABEL          "Priority"
#define VLAN_DEI_LABEL          "DEI"
#define VLAN_VID_LABEL          "VLAN ID"
#define VLAN_ETHERTYPE_LABEL    "EtherType"

/* 
    IMPORTANT: The TPID is excluded because it has already been parsed as part of the Ethernet frame 
    (it serves as the EtherType in the Ethernet header). Although I included it in the visualization command, it is 
    technically redundant.
*/

#define VLAN_TCI(pkt)               (((uint16_t)pkt[0] << 8) | (uint16_t)pkt[1])
#define VLAN_PCP                    0xe000
#define VLAN_DEI                    0x1000
#define VLAN_VID                    0x0fff
#define VLAN_ETHERTYPE(pkt)         (((uint16_t)pkt[2] << 8) | (uint16_t)pkt[3])

#define VLAN_HDR_LEN                4  /* should be 6, but the 2 bytes TPID field is not considered, so 6 - 2 = 4 bytes */


protocol_info dissect_vlan(const uint8_t *pkt, size_t pkt_len);

#endif