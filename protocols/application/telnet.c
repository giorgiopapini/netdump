#include "telnet.h"

#include <stdio.h>
#include <string.h>

#include "../../utils/visualizer.h"
#include "../../utils/protocol.h"


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

void print_telnet_cmd(const uint8_t cmd, const uint8_t sub_cmd);
void visualize_telnet_cmd(const uint8_t cmd, const uint8_t sub_cmd);
size_t visualize_telnet_data(const uint8_t *pkt, size_t telnet_len);
void print_telnet_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_telnet_hdr(const uint8_t *pkt, size_t pkt_len);

void print_telnet_cmd(const uint8_t cmd, const uint8_t sub_cmd) {
    printf("%s (", telnet_commands[cmd - TELNET_COMMAND_OFFSET]);
    if (sub_cmd <= TELNET_END_I_OPTIONS) printf("%s", telnet_options_first_segment[sub_cmd]);
    else if (sub_cmd >= TELNET_START_II_OPTIONS && 
            sub_cmd <= TELNET_END_II_OPTIONS) printf("%s", telnet_options_second_segment[sub_cmd - TELNET_START_II_OPTIONS]);
    else if (sub_cmd == TELNET_EXTENDED_OPT_LIST) printf("Extended-Options-List");
    else printf("Unassigned");
}

void visualize_telnet_cmd(const uint8_t cmd, const uint8_t sub_cmd) {
    char res[512] = "";
    
    strncat(res, telnet_commands[cmd - TELNET_COMMAND_OFFSET], sizeof(res) - strlen(res) - 1);
    strncat(res, " (", sizeof(res) - strlen(res) - 1);
    if (sub_cmd <= TELNET_END_I_OPTIONS) strncat(res, telnet_options_first_segment[sub_cmd], sizeof(res) - strlen(res) - 1);
    else if (sub_cmd >= TELNET_START_II_OPTIONS && sub_cmd <= TELNET_END_II_OPTIONS) 
        strncat(res, telnet_options_second_segment[sub_cmd - TELNET_START_II_OPTIONS], sizeof(res) - strlen(res) - 1);
    else if (sub_cmd == TELNET_EXTENDED_OPT_LIST) strncat(res, "Extended-Options-List", sizeof(res) - strlen(res) - 1);
    else strncat(res, "Unassigned", sizeof(res) - strlen(res) - 1);
    strncat(res, ")", sizeof(res) - strlen(res) - 1);

    print_field(TELNET_COMMAND_LABEL, res, 0);
}

size_t visualize_telnet_data(const uint8_t *pkt, size_t telnet_len) {
    char buff[512] = "";
    char temp[4];
    size_t i = 0;
    size_t buff_len;

    buff_len = 0;
    for (i = 0; i < telnet_len; i ++) {
        if (buff_len >= sizeof(buff) - 1) break;
        
        if (pkt[i] == '\r' && (i < telnet_len - 1) && pkt[i + 1] == '\n') {
            strncat(buff, "\\r\\n", sizeof(buff) - strlen(buff) - 1);
            buff_len = strlen(buff);

            if (i < telnet_len -2) {
                if (pkt[i + 2] != TELNET_IAC) strncat(buff, ", ", sizeof(buff) - buff_len - 1);
            }
            i ++;
        }
        else {
            snprintf(temp, sizeof(temp), "%c", pkt[i]);
            strncat(buff, temp, sizeof(buff) - buff_len - 1);
            buff_len = strlen(buff);
        }
    }
    print_field(TELNET_DATA_LABEL, buff, 0);
    return i;
}

void print_telnet_hdr(const uint8_t *pkt, size_t pkt_len) {
    int data_segment_len = 0;
    size_t i;

    if (!pkt) return;

    /*
        +3 because (1 byte = flag 0xff, 1 byte = command, 1 byte = subcommand)
        !(i > pkt_len - 4) to ensure that inside the for loop the code doesn't access an index greater then the actual length of the arr
    */
    for (i = 0; (i < pkt_len) && !(i > pkt_len - 4);) {
        if (TELNET_IAC == pkt[i]) {
            print_telnet_cmd(pkt[i + 1], pkt[i + 2]);
        
            if ((pkt_len >= 4) && (i <= pkt_len - 4)) printf("), ");
            else printf(")");

            i += 3;
            data_segment_len = 0;
        }
        else {
            if (0 == data_segment_len) printf("data: (");
            
            if ('\r' == pkt[i] && '\n' == pkt[i + 1]) {
                printf("\\r\\n");
                if (pkt_len >= 3 && i <= pkt_len - 3) {
                    if (TELNET_IAC != pkt[i + 2]) printf(", ");
                    else printf("), ");
                }
                else printf(")");

                i += 2;  /* += 2 because im considering 2 bytes in one iteration, so i skip one iteration */
                data_segment_len += 2;
            }
            else {
                printf("%c", pkt[i]);
                if (pkt_len >= 2 && i <= pkt_len - 2) {
                    if (TELNET_IAC == pkt[i + 1]) printf("), ");
                }
                else printf(")");

                data_segment_len ++;
                i ++;
            }
        }
    }
}

void visualize_telnet_hdr(const uint8_t *pkt, size_t pkt_len) {
    size_t i;

    if (!pkt) return;

    start_printing();
    for (i = 0; i < pkt_len;) {
        if (TELNET_IAC == pkt[i]) {
            visualize_telnet_cmd(pkt[i + 1], pkt[i + 2]);
            i += 3;
        }
        else i += visualize_telnet_data(pkt, pkt_len);
    }
    end_printing();
}

protocol_info dissect_telnet(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    if (!pkt) return NO_ENCAP_PROTO;
    
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_telnet_hdr, visualize_telnet_hdr);
    return NO_ENCAP_PROTO;
}