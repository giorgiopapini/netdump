#include "net_ports.h"

#include "application/http.h"

protocol_handler net_ports[] = {
    { PORT_HTTP,            PROTOCOL_LAYER_APPLICATION, dissect_http, "HTTP" },
    { PORT_HTTPS,           PROTOCOL_LAYER_APPLICATION, NULL, "HTTPS" },
    { PORT_FTP,             PROTOCOL_LAYER_APPLICATION, NULL },
    { PORT_SSH,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_TELNET,          PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_SMTP,            PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_DNS,             PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_DHCP_CLIENT,     PROTOCOL_LAYER_NONE, NULL, NULL },
    { PORT_DHCP_SERVER,     PROTOCOL_LAYER_NONE, NULL, NULL },
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
    { 0,                    PROTOCOL_LAYER_NONE, NULL, NULL },
};