#include <stdio.h>

#include "vlan.h"
#include "../../utils/visualizer.h"
#include "../proto_tables_nums.h"


void print_vlan_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_vlan_hdr(const uint8_t *pkt, size_t pkt_len);

void print_vlan_hdr(const uint8_t *pkt, size_t pkt_len) {
    (void)pkt_len;
    
    printf(
        "TCI: 0x%04x, ethertype: 0x%04x",
        VLAN_TCI(pkt),
        VLAN_ETHERTYPE(pkt)
    );
}

void visualize_vlan_hdr(const uint8_t *pkt, size_t pkt_len) {
    char priority[6];
    char dei[6];
    char vlan_id[5];  /* 12 bits, max int value = 4095; so "4095'\0'" = 5 chars */
    char ethertype[7];  /* 0xXXXX'\0' */
    (void)pkt_len;
    
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

protocol_info dissect_vlan(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_vlan_hdr, visualize_vlan_hdr);
    return (protocol_info){ .protocol = VLAN_ETHERTYPE(pkt), .offset = VLAN_LEN, .proto_table_num = ETHERTYPES };
}