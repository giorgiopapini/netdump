#ifndef ETHER_H
#define ETHER_H

#include <stdint.h>
#include "../../utils/protocol.h"
#include "../../protocol_handler.h"

#define ETHER_HEADER_LABEL      "Ether Header"
#define DEST_ADDR_LABEL         "Destination MAC Address"
#define SRC_ADDR_LABEL          "Source MAC Address"
#define ETHERTYPE_LABEL         "EtherType"

#define DEST_ADDR(pkt)          (pkt)
#define SRC_ADDR(pkt)           (pkt + 6)
#define ETHERTYPE(pkt)          (*((uint16_t *)(pkt + 12)))

#define ETHER_LEN               14


protocol_info dissect_ether(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

/* 
    const uint8_t *pkt is not used in ether_hdr_len() function. It needs to be described anyways because the function pointer
    needs to be stored inside of a protocol_info table and that structure is described in such a way that it accepts only functions
    defined in this way
*/

#endif