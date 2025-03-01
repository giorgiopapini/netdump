#include <stdio.h>
#include <arpa/inet.h>

#include "arp.h"
#include "../ethertypes.h"
#include "../../utils/formats.h"
#include "../../utils/visualizer.h"


void print_arp_hdr(const uint8_t *pkt, uint32_t len) {
    uint16_t operation = ntohs(OPERATION(pkt));

    if (operation == ARP_REQUEST) {
        printf("ARP who-has ");
        print_ipv4(ntohl(TARGET_P_ADDR(pkt)));
        printf(" tell ");
        print_ipv4(ntohl(SENDER_P_ADDR(pkt)));
    }
    else if (operation == ARP_RESPONSE) {
        printf("ARP reply ");
        print_ipv4(ntohl(SENDER_P_ADDR(pkt)));
        printf(" is-at ");
        print_mac(SENDER_HW_ADDR(pkt));
    }
    else if (operation == RARP_REQUEST) {
        printf("RARP who-is ");
        print_mac(TARGET_HW_ADDR(pkt));
        printf(" tell ");
        print_mac(SENDER_HW_ADDR(pkt));
    }
    else if (operation == RARP_RESPONSE) {
        printf("RARP reply ");
        print_mac(TARGET_HW_ADDR(pkt));
        printf(" is-at ");
        print_ipv4(ntohl(TARGET_P_ADDR(pkt)));
    }
    else printf("(unrecognized operation = %u)", operation);
}

void visualize_arp_hdr(const uint8_t *pkt, uint32_t len) {
    char hw_type[7];
    char p_type[7];
    char hw_len[5];
    char p_len[5];
    char operation[7];
    char sender_hw_addr[MAC_ADDR_STR_LEN];
    char sender_p_addr[IP_ADDR_STR_LEN];
    char target_hw_addr[MAC_ADDR_STR_LEN];
    char target_p_addr[IP_ADDR_STR_LEN];

    snprintf(hw_type, sizeof(hw_type), "0x%04x", ntohs(HW_TYPE(pkt)));
    snprintf(p_type, sizeof(p_type), "0x%04x", ntohs(P_TYPE(pkt)));
    snprintf(hw_len, sizeof(hw_len), "0x%02x", HW_LEN(pkt));
    snprintf(p_len, sizeof(p_len), "0x%02x", P_LEN(pkt));
    snprintf(operation, sizeof(operation), "0x%04x", ntohs(OPERATION(pkt)));
    snprintf(sender_hw_addr, sizeof(sender_hw_addr), MAC_ADDR_FORMAT, MAC_TO_STR(SENDER_HW_ADDR(pkt)));
    snprintf(sender_p_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(SENDER_P_ADDR(pkt))));
    snprintf(target_hw_addr, sizeof(target_hw_addr), MAC_ADDR_FORMAT, MAC_TO_STR(TARGET_HW_ADDR(pkt)));
    snprintf(target_p_addr, IP_ADDR_STR_LEN, IP_ADDR_FORMAT, IP_TO_STR(ntohl(TARGET_P_ADDR(pkt))));

    start_printing();
    print_field(HW_TYPE_LABEL, hw_type, 0);
    print_field(P_TYPE_LABEL, p_type, 0);
    print_field(HW_LEN_LABEL, hw_len, 0);
    print_field(P_LEN_LABEL, p_len, 0);
    print_field(OPERATION_LABEL, operation, 0);
    print_field(SENDER_HW_ADDR_LABEL, sender_hw_addr, 0);
    print_field(SENDER_P_ADDR_LABEL, sender_p_addr, 0);
    print_field(TARGET_HW_ADDR_LABEL, target_hw_addr, 0);
    print_field(TARGET_P_ADDR_LABEL, target_p_addr, 0);
    end_printing();
}

protocol_info dissect_arp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_arp_hdr, visualize_arp_hdr);
    return NO_ENCAP_PROTO;
}