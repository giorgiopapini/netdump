#include "transport_handler.h"
#include "../ipprotos.h"
#include "transport/icmp.h"
#include "transport/tcp.h"

protocol_info transport_protocols[] = {
    { IPPROTO_ICMP, icmp_hdr_len, 0, 0, 0, print_icmp_hdr, visualize_icmp_hdr },
    { IPPROTO_TCP, tcp_hdr_len, 0, 0, 0, print_tcp_hdr, visualize_tcp_hdr }
};

protocol_info dissect_transport(int protocol_type) {
    int i;
    for (i = 0; i < sizeof(transport_protocols) / sizeof(transport_protocols[0]); i ++) {
        if (transport_protocols[i].protocol == protocol_type) return transport_protocols[i]; 
    }
    return (protocol_info){ 0, 0, 0, 0, 0, NULL, NULL };
}