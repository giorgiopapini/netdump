#include "net_ports.h"

#include <stddef.h>

#include "../utils/protocol.h"
#include "application/dhcp.h"
#include "application/http.h"
#include "application/ftp.h"
#include "application/dns.h"
#include "application/telnet.h"

protocol_handler net_ports[] = {
    { PORT_HTTP,            PROTOCOL_LAYER_APPLICATION, dissect_http, "HTTP" },
    { PORT_HTTPS,           PROTOCOL_LAYER_APPLICATION, NULL, "HTTPS" },
    { PORT_FTP,             PROTOCOL_LAYER_APPLICATION, dissect_ftp, "FTP" },
    { PORT_SSH,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_TELNET,          PROTOCOL_LAYER_APPLICATION, dissect_telnet, "Telnet" },
    { PORT_SMTP,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_DNS,             PROTOCOL_LAYER_APPLICATION, dissect_dns, "DNS" },
    { PORT_DHCP_CLIENT,     PROTOCOL_LAYER_APPLICATION, dissect_dhcp, "DHCP Client" },
    { PORT_DHCP_SERVER,     PROTOCOL_LAYER_APPLICATION, dissect_dhcp, "DHCP Server" },
    { PORT_POP3,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_IMAP,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_SNMP,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_LDAP,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_RDP,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_NTP,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_BGP,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_IRC,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_SYSLOG,          PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_SMB,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_MQTT,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_RTSP,            PROTOCOL_LAYER_NONE, NULL, NULL },
    NULL_PROTO_HANDLER,
};