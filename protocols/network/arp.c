#include <stdio.h>
#include <arpa/inet.h>

#include "arp.h"
#include "../../ethertypes.h"
#include "../../utils/formats.h"

void print_arp_hdr(const uint8_t *pkt) {
    uint16_t operation = ntohs(OPERATION(pkt));

    printf("arp ");
    if (operation == ARP_REQUEST) {
        printf(ARP_REQUEST_STR " ");
        print_ipv4(ntohl(TARGET_PROTOCOL_ADDR(pkt)));
        printf(" tell ");
        print_ipv4(ntohl(SENDER_PROTOCOL_ADDR(pkt)));
    }
    else if (operation == ARP_RESPONSE) {
        printf(ARP_RESPONSE_STR " ");
        print_ipv4(ntohl(SENDER_PROTOCOL_ADDR(pkt)));
        printf(" is-at ");
        print_mac(SENDER_HARDWARE_ADDR(pkt));
    }
}

void visualize_arp_hdr(const uint8_t *pkt) {

}