#include "../ethertypes.h"
#include "network_handler.h"
#include "network/ip.h"  /* find a way to import all the protocols using one single include directive */
#include <arpa/inet.h> 

protocol_info network_protocols[] = {
    { ETHERTYPE_IP, sizeof(ip_hdr), { .start = 9, .len = 1, .ntoh = 0 }, print_ip_header }
};

protocol_info dissect_network(int protocol_type) {
    int i;
    for (i = 0; i < sizeof(network_protocols) / sizeof(network_protocols[0]); i ++) {
        if (network_protocols[i].protocol == protocol_type) return network_protocols[i]; 
    }
    return (protocol_info){ 0, 0, 0, 0, 0, NULL };
}