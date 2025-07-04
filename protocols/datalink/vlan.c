#include "vlan.h"

#include <stdio.h>

#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static void _print_vlan_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_vlan_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _print_vlan_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    if (!pkt || pkt_len < hdr_len) return;
    
    printf(
        "TCI: 0x%04x, ethertype: 0x%04x",
        VLAN_TCI(pkt),
        VLAN_ETHERTYPE(pkt)
    );
}

static void _visualize_vlan_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char priority[6];
    char dei[6];
    char vlan_id[5];  /* 12 bits, max int value = 4095; so "4095'\0'" = 5 chars */
    char ethertype[7];  /* 0xXXXX'\0' */
    
    if (!pkt || pkt_len < hdr_len) return;
    
    snprintf(priority, sizeof(priority), "%u", VLAN_TCI(pkt) & VLAN_PCP);
    snprintf(dei, sizeof(dei), "%u", VLAN_TCI(pkt) & VLAN_DEI);
    snprintf(vlan_id, sizeof(vlan_id), "%u", VLAN_TCI(pkt) & VLAN_VID);
    snprintf(ethertype, sizeof(ethertype), "0x%04x", VLAN_ETHERTYPE(pkt));

    start_printing();
    print_field(VLAN_PCP_LABEL, priority, 0);
    print_field(VLAN_DEI_LABEL, dei, 0);
    print_field(VLAN_VID_LABEL, vlan_id, 0);
    print_field(VLAN_ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}

protocol_info dissect_vlan(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < VLAN_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func = _print_vlan_hdr,
        .visualize_protocol_func = _visualize_vlan_hdr,
        .hdr_len = VLAN_HDR_LEN,
        .encap_protocol = VLAN_ETHERTYPE(pkt),
        .encap_proto_table_num = ETHERTYPES
    };
}