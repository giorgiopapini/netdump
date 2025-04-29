#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ftp.h"
#include "../../utils/visualizer.h"


void parse_ftp_response(const uint8_t *response, size_t pkt_len, char *code, char *message) {
    if (!response || !code || !message == 0) return;
    if (pkt_len < 5) return;

    size_t remaining;
    size_t copy_len;
    size_t msg_len = strlen(message);
    size_t safe_len = 0;

    while (safe_len < pkt_len && response[safe_len] != '\0') safe_len ++;
    if (safe_len == pkt_len) return;

    strncpy(code, (const char *)response, 3);
    code[3] = '\0';

    remaining = pkt_len - 4;
    copy_len = (remaining < msg_len - 1) ? remaining : msg_len - 1;

    strncpy(message, (const char *)(response + 4), copy_len);
    message[copy_len] = '\0';

    if (msg_len >= 2 && message[msg_len - 2] == '\r' && message[msg_len - 1] == '\n') message[msg_len - 2] = '\0';
}

void parse_ftp_request(const uint8_t *request, size_t pkt_len, char *command, char *argument) {
    if (!request || pkt_len == 0 || !command || !argument == 0) return;

    size_t i = 0;
    size_t arg_len;
    size_t command_size = strlen(command);
    size_t argument_size = strlen(argument);

    while (i < pkt_len && request[i] != ' ' && request[i] != '\0' && i < command_size - 1 && i < 4) {
        command[i] = request[i];
        i ++;
    }
    command[i] = '\0';

    if (i < pkt_len && request[i] == ' ') {
        i ++;
        arg_len = pkt_len - i;

        if (arg_len >= 2 && request[i + arg_len - 2] == '\r' && request[i + arg_len - 1] == '\n') {
            arg_len -= 2;
        }
        if (arg_len >= argument_size) {
            arg_len = argument_size - 1;
        }

        memcpy(argument, request + i, arg_len);
        argument[arg_len] = '\0';
    }
    else argument[0] = '\0';
}

void print_ftp_hdr(const uint8_t *pkt, size_t pkt_len) {
    (void)pkt_len;

    size_t i = 0;
    if (!pkt || pkt[0] == '\0') return;
    
    if (isdigit(pkt[0])) printf("Response: ");
    else if (isupper(pkt[0])) printf("Request: ");

    while (pkt[i] != '\r') {
        printf("%c", pkt[i]);
        i ++;
    }
}

void visualize_ftp_hdr(const uint8_t *pkt, size_t pkt_len) {
    (void)pkt_len;
    
    char str1[FTP_HEADER_MAX_LEN];
    char str2[FTP_HEADER_MAX_LEN];
    if (!pkt || pkt[0] == '\0') return;
    
    start_printing();
    if (isdigit(pkt[0])) {
        parse_ftp_response(pkt, pkt_len, str1, str2);
        print_field(FTP_TYPE_LABEL, "Response", 0);
        if (strlen(str1) > 0) print_field(FTP_RES_CODE_LABEL, str1, 0);
        if (strlen(str2) > 0) print_field(FTP_TYPE_LABEL, str2, 0);
    }
    else if (isupper(pkt[0])) {
        parse_ftp_request(pkt, pkt_len, str1, str2);
        print_field(FTP_TYPE_LABEL, "Response", 0);
        if (strlen(str1) > 0) print_field(FTP_COMMAND_LABEL, str1, 0);
        if (strlen(str2) > 0) print_field(FTP_ARGUMENT_LABEL, str2, 0);
    }
    end_printing();
}

protocol_info dissect_ftp(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_ftp_hdr, visualize_ftp_hdr);
    return NO_ENCAP_PROTO;
}