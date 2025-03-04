#include <stdio.h>
#include <string.h>

#include "telnet.h"
#include "../../utils/visualizer.h"


const char *telnet_commands[] = {
    "SE",
    "NOP",
    "Data Mark",
    "Break",
    "Interrupt Process",
    "Abort Output",
    "Are You There",
    "Erase Character",
    "Erase Line",
    "Go Ahead",
    "SB",
    "WILL",
    "WON'T",
    "DO",
    "DON'T",
    "IAC"
};

/* from 0 to 49 */
const char *telnet_options_first_segment[] = {
    "Binary Transmission",
    "Echo",
    "Reconnection",
    "Suppress Go Ahead",
    "Approx Message Size Negotiation",
    "Status",
    "Timing Mark",
    "Remote Controlled Trans and Echo",
    "Output Line Width",
    "Output Page Size",
    "Output Carriage-Return Disposition",
    "Output Horizontal Tab Stops",
    "Output Horizontal Tab Disposition",
    "Output Formfeed Disposition",
    "Output Vertical Tabstops",
    "Output Vertical Tab Disposition",
    "Output Linefeed Disposition",
    "Extended ASCII",
    "Logout",
    "Byte Macro",
    "Data Entry Terminal",
    "SUPDUP",
    "SUPDUP Output",
    "Send Location",
    "Terminal Type",
    "End of Record",
    "TACACS User Identification",
    "Output Marking",
    "Terminal Location Number",
    "Telnet 3270 Regime",
    "X.3 PAD",
    "Negotiate About Window Size",
    "Terminal Speed",
    "Remote Flow Control",
    "Linemode",
    "X Display Location",
    "Environment Option",
    "Authentication Option",
    "Encryption Option",
    "New Environment Option",
    "TN3270E",
    "XAUTH",
    "CHARSET",
    "Telnet Remote Serial Port (RSP)",
    "Com Port Control Option",
    "Telnet Suppress Local Echo",
    "Telnet Start TLS",
    "KERMIT",
    "SEND-URL",
    "FORWARD_X"
};

/* from 138 to 140 */
const char *telnet_options_second_segment[] = {
    "TELOPT PRAGMA LOGON",
    "TELOPT SSPI LOGON",
    "TELOPT PRAGMA HEARTBEAT"
};

void print_telnet_cmd(const uint8_t cmd, const uint8_t sub_cmd) {
    printf("%s (", telnet_commands[cmd - TELNET_COMMAND_OFFSET]);
    if (sub_cmd <= TELNET_END_I_OPTIONS) printf("%s", telnet_options_first_segment[sub_cmd]);
    else if (sub_cmd >= TELNET_START_II_OPTIONS && 
            sub_cmd <= TELNET_END_II_OPTIONS) printf("%s", telnet_options_second_segment[sub_cmd]);
    else if (sub_cmd == TELNET_EXTENDED_OPT_LIST) printf("Extended-Options-List");
    else printf("Unassigned");
}

void visualize_telnet_cmd(const uint8_t cmd, const uint8_t sub_cmd) {
    char res[512] = "";
    
    strncat(res, telnet_commands[cmd - TELNET_COMMAND_OFFSET], sizeof(res) - strlen(res) - 1);
    strncat(res, " (", sizeof(res) - strlen(res) - 1);
    if (sub_cmd <= TELNET_END_I_OPTIONS) strncat(res, telnet_options_first_segment[sub_cmd], sizeof(res) - strlen(res) - 1);
    else if (sub_cmd >= TELNET_START_II_OPTIONS && 
            sub_cmd <= TELNET_END_II_OPTIONS) strncat(res, telnet_options_second_segment[sub_cmd], sizeof(res) - strlen(res) - 1);
    else if (sub_cmd == TELNET_EXTENDED_OPT_LIST) strncat(res, "Extended-Options-List", sizeof(res) - strlen(res) - 1);
    else strncat(res, "Unassigned", sizeof(res) - strlen(res) - 1);
    strncat(res, ")", sizeof(res) - strlen(res) - 1);

    print_field(TELNET_COMMAND_LABEL, res, 0);
}

int visualize_telnet_data(const uint8_t *pkt, uint32_t telnet_len) {
    char buff[512] = "";
    size_t i = 0;
    size_t buff_len = 0;

    for (i = 0; i < telnet_len; i++) {
        if (buff_len >= sizeof(buff) - 1) break;
        
        if (pkt[i] == '\r' && (i + 1 < telnet_len) && pkt[i + 1] == '\n') {
            strncat(buff, "\\r\\n", sizeof(buff) - strlen(buff) - 1);
            buff_len = strlen(buff);

            if ((i + 2) < telnet_len) {
                if (pkt[i + 2] != TELNET_IAC) strncat(buff, ", ", sizeof(buff) - buff_len - 1);
            }
            i ++;
        } else {
            char temp[4];  /* small buffer to store one character */
            snprintf(temp, sizeof(temp), "%c", pkt[i]);
            strncat(buff, temp, sizeof(buff) - buff_len - 1);
            buff_len = strlen(buff);
        }
    }
    print_field(TELNET_DATA_LABEL, buff, 0);
    return i;
}

void print_telnet_hdr(const uint8_t *pkt, uint32_t len) {
    int data_segment_len = 0;
    int i;

    for (i = 0; i < len;) {  /* +3 because (1 byte = flag 0xff, 1 byte = command, 1 byte = subcommand) */
        if (TELNET_IAC == pkt[i]) {
            print_telnet_cmd(pkt[i + 1], pkt[i + 2]);
        
            if ((i + 3) >= len) printf(")");
            else printf("), ");

            i += 3;
            data_segment_len = 0;
        }
        else {
            if (0 == data_segment_len) printf("data: (");
            
            if ('\r' == pkt[i] && '\n' == pkt[i + 1]) {
                printf("\\r\\n");
                if ((i + 2) < len) {
                    if (TELNET_IAC != pkt[i + 2]) printf(", ");
                    else printf("), ");
                }
                else printf(")");

                i += 2;  /* += 2 because im considering 2 bytes in one iteration, so i skip one iteration */
                data_segment_len += 2;
            }
            else {
                printf("%c", pkt[i]);
                if (i + 1 < len) {
                    if (TELNET_IAC == pkt[i + 1]) printf("), ");
                }
                else printf(")");

                data_segment_len ++;
                i ++;
            }
        }
    }
}

void visualize_telnet_hdr(const uint8_t *pkt, uint32_t len) {
    int data_segment_len = 0;
    int i;

    start_printing();
    for (i = 0; i < len;) {
        if (TELNET_IAC == pkt[i]) {
            visualize_telnet_cmd(pkt[i + 1], pkt[i + 2]);
            i += 3;
        }
        else i += visualize_telnet_data(pkt, len);
    }
    end_printing();
}

protocol_info dissect_telnet(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_telnet_hdr, visualize_telnet_hdr);
    return NO_ENCAP_PROTO;
}