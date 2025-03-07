#ifndef NET_PORTS_H
#define NET_PORTS_H

#include "../utils/protocol.h"

#ifndef PORT_HTTP
#define PORT_HTTP       80      /* HTTP */
#endif
#ifndef PORT_HTTPS
#define PORT_HTTPS      443     /* HTTPS */
#endif
#ifndef PORT_FTP
#define PORT_FTP        21      /* FTP */
#endif
#ifndef PORT_SSH
#define PORT_SSH        22      /* SSH */
#endif
#ifndef PORT_TELNET
#define PORT_TELNET     23      /* Telnet */
#endif
#ifndef PORT_SMTP
#define PORT_SMTP       25      /* SMTP */
#endif
#ifndef PORT_DNS
#define PORT_DNS        53      /* DNS */
#endif
#ifndef PORT_DHCP_CLIENT
#define PORT_DHCP_CLIENT 68     /* DHCP Client */
#endif
#ifndef PORT_DHCP_SERVER
#define PORT_DHCP_SERVER 67     /* DHCP Server */
#endif
#ifndef PORT_POP3
#define PORT_POP3       110     /* POP3 */
#endif
#ifndef PORT_IMAP
#define PORT_IMAP       143     /* IMAP */
#endif
#ifndef PORT_SNMP
#define PORT_SNMP       161     /* SNMP */
#endif
#ifndef PORT_LDAP
#define PORT_LDAP       389     /* LDAP */
#endif
#ifndef PORT_RDP
#define PORT_RDP        3389    /* RDP */
#endif
#ifndef PORT_NTP
#define PORT_NTP        123     /* NTP */
#endif
#ifndef PORT_BGP
#define PORT_BGP        179     /* BGP */
#endif
#ifndef PORT_IRC
#define PORT_IRC        6667    /* IRC */
#endif
#ifndef PORT_SYSLOG
#define PORT_SYSLOG     514     /* Syslog */
#endif
#ifndef PORT_SMB
#define PORT_SMB        445     /* SMB */
#endif
#ifndef PORT_MQTT
#define PORT_MQTT       1883    /* MQTT */
#endif
#ifndef PORT_RTSP
#define PORT_RTSP       554     /* RTSP */
#endif

#define IS_WELL_DEFINED_PORT(port)          (port < 1023)  /* 1023 is the last so called "well defined" port */


extern protocol_handler net_ports[];

#endif