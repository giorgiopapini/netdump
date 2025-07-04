#include "snap.h"

#include <stdio.h>

#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"

static void _print_snap_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_snap_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _print_snap_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    if (!pkt || pkt_len < hdr_len) return;
    
    printf(
        "oui: %02x:%02x:%02x, ethertype: 0x%04x", 
        SNAP_OUI(pkt, 0), SNAP_OUI(pkt, 1), SNAP_OUI(pkt, 2),
        SNAP_TYPE(pkt)
    );
}

static void _visualize_snap_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char oui[9];  /* 00:00:00'\0' 9 chars */
    char ethertype[7];
    
    if (!pkt || pkt_len < hdr_len) return;
    
    snprintf(oui, sizeof(oui), "%02x:%02x:%02x", SNAP_OUI(pkt, 0), SNAP_OUI(pkt, 1), SNAP_OUI(pkt, 2));
    snprintf(ethertype, sizeof(ethertype), "0x%04x", SNAP_TYPE(pkt));

    start_printing();
    print_field(SNAP_OUI_LABEL, oui, 0);
    print_field(SNAP_ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}

protocol_info dissect_snap(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < SNAP_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_snap_hdr,
        .visualize_protocol_func = _visualize_snap_hdr,
        .hdr_len = SNAP_HDR_LEN,
        .encap_protocol = SNAP_TYPE(pkt),
        .encap_proto_table_num = ETHERTYPES
    };
}