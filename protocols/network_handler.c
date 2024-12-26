#include "network_handler.h"
#include "../ethertypes.h"
#include "network/ip.h"  /* find a way to import all the protocols using one single include directive */ 
#include "network/arp.h"

protocol_info network_protocols[] = {
    { ETHERTYPE_IP, ip_hdr_len, { .start = 9, .len = 1, .ntoh = 0 }, print_ip_hdr, visualize_ip_hdr },
    { ETHERTYPE_ARP, arp_hdr_len, 0, 0, 0, print_arp_hdr, visualize_arp_hdr },
    { ETHERTYPE_RARP, arp_hdr_len, 0, 0, 0, print_arp_hdr, visualize_arp_hdr }
};

protocol_info dissect_network(int protocol_type) {
    int i;
    for (i = 0; i < sizeof(network_protocols) / sizeof(network_protocols[0]); i ++) {
        if (network_protocols[i].protocol == protocol_type) return network_protocols[i]; 
    }
    return (protocol_info){ 0, 0, 0, 0, 0, NULL, NULL };
}