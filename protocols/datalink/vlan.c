#include <stdio.h>
#include <arpa/inet.h>

#include "vlan.h"
#include "../ethertypes.h"
#include "../../utils/visualizer.h"


void print_vlan_hdr(const uint8_t *pkt, uint32_t len) {
    printf(
        "TCI: 0x%04x, ethertype: 0x%04x",
        ntohs(VLAN_TCI(pkt)),
        ntohs(VLAN_ETHERTYPE(pkt))
    );
}

void visualize_vlan_hdr(const uint8_t *pkt, uint32_t len) {
    char priority[2];  /* 3 bits, max int value = 7; */
    char dei[2];  /* 1 bit */
    char vlan_id[5];  /* 12 bits, max int value = 4095; so "4095'\0'" = 5 chars */
    char ethertype[7];  /* 0xXXXX'\0' */

    snprintf(priority, sizeof(priority), "%u", ntohs(VLAN_TCI(pkt)) & VLAN_PCP);
    snprintf(dei, sizeof(dei), "%u", ntohs(VLAN_TCI(pkt)) & VLAN_DEI);
    snprintf(vlan_id, sizeof(vlan_id), "%u", ntohs(VLAN_TCI(pkt)) & VLAN_VID);
    snprintf(ethertype, sizeof(ethertype), "0x%04x", ntohs(VLAN_ETHERTYPE(pkt)));

    start_printing();
    print_hdr_info(VLAN_HEADER_LABEL, NULL);
    print_field(VLAN_PCP_LABEL, priority, 0);
    print_field(VLAN_DEI_LABEL, dei, 0);
    print_field(VLAN_VID_LABEL, vlan_id, 0);
    print_field(VLAN_ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}

protocol_info dissect_vlan(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, proto_name, print_vlan_hdr, visualize_vlan_hdr);
    return (protocol_info){ .protocol = ntohs(VLAN_ETHERTYPE(pkt)), .offset = VLAN_LEN, .table = ethertypes };
}