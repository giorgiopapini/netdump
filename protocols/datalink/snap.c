#include <stdio.h>

#include "snap.h"
#include "../ethertypes.h"
#include "../../utils/visualizer.h"


void print_snap_hdr(const uint8_t *pkt) {
    printf(
        "oui: %02x:%02x:%02x, ethertype: 0x%04x", 
        SNAP_OUI(pkt, 0), SNAP_OUI(pkt, 1), SNAP_OUI(pkt, 2),
        ntohs(SNAP_TYPE(pkt))
    );
}

void visualize_snap_hdr(const uint8_t *pkt) {
    char oui[9];  /* 00:00:00'\0' 9 chars */
    char ethertype[7];

    snprintf(oui, sizeof(oui), "%02x:%02x:%02x", SNAP_OUI(pkt, 0), SNAP_OUI(pkt, 1), SNAP_OUI(pkt, 2));
    snprintf(ethertype, sizeof(ethertype), "0x%04x", ntohs(SNAP_TYPE(pkt)));

    start_printing();
    print_hdr_info(SNAP_HEADER_LABEL, NULL);
    print_field(SNAP_OUI_LABEL, oui, 0);
    print_field(SNAP_ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}

protocol_info dissect_snap(const uint8_t *pkt, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_snap_hdr, visualize_snap_hdr);
    return (protocol_info){ .protocol = ntohs(SNAP_TYPE(pkt)), .offset = SNAP_LEN, .table = ethertypes };
}