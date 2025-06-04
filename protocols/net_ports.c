#include "net_ports.h"

#include <stddef.h>

#include "../utils/hashmap.h"
#include "../utils/protocol.h"
#include "application/dhcp.h"
#include "application/http.h"
#include "application/ftp.h"
#include "application/dns.h"
#include "application/telnet.h"


hashmap *net_ports = NULL;

void load_net_ports(void) {
    net_ports = create_hashmap(NET_PORTS_BUCKETS_NUM);
    ADD_PROTO_HANDLER_ENTRY(net_ports, PORT_HTTP, PROTOCOL_LAYER_APPLICATION, dissect_http, "HTTP");
    ADD_PROTO_HANDLER_ENTRY(net_ports, PORT_FTP, PROTOCOL_LAYER_APPLICATION, dissect_ftp, "FTP");
    ADD_PROTO_HANDLER_ENTRY(net_ports, PORT_TELNET, PROTOCOL_LAYER_APPLICATION, dissect_telnet, "Telnet");
    ADD_PROTO_HANDLER_ENTRY(net_ports, PORT_DNS, PROTOCOL_LAYER_APPLICATION, dissect_dns, "DNS");
    ADD_PROTO_HANDLER_ENTRY(net_ports, PORT_DHCP_CLIENT, PROTOCOL_LAYER_APPLICATION, dissect_dhcp, "DHCP Client");
    ADD_PROTO_HANDLER_ENTRY(net_ports, PORT_DHCP_SERVER, PROTOCOL_LAYER_APPLICATION, dissect_dhcp, "DHCP Server");
}