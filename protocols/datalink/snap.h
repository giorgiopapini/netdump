#ifndef SNAP_H
#define SNAP_H

#include <stdint.h>
#include "../../utils/protocol.h"

#define SNAP_HEADER_LABEL       "SNAP Header"
#define SNAP_OUI_LABEL          "OUI"
#define SNAP_ETHERTYPE_LABEL    "EtherType"

#define SNAP_OUI(pkt, index)    (*(uint8_t *)(pkt + index))
#define SNAP_TYPE(pkt)          (*(uint16_t *)(pkt + 3))

#define SNAP_LEN                5


protocol_info dissect_snap(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif