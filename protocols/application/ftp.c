#include "ftp.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static void _parse_ftp_response(const uint8_t *response, size_t pkt_len, char *code, size_t code_size, char *message, size_t message_size);
static void _parse_ftp_request(const uint8_t *request, size_t pkt_len, char *command, size_t command_size, char *argument, size_t argument_size);
static void _print_ftp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_ftp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _parse_ftp_response(const uint8_t *response, size_t pkt_len, char *code, size_t code_size, char *message, size_t message_size) {
    size_t code_len;
    size_t i;
    size_t msg_end;
    size_t copy_len;

    if (!response || !code || !message || code_size < 4 || message_size == 0) return;
    if (pkt_len < 5) return;

    if (pkt_len < 3) return;
    code_len = (code_size > 3) ? 3 : code_size - 1;
    memcpy(code, response, code_len);
    code[code_len] = '\0';

    i = 4;
    msg_end = i;
    while (msg_end < pkt_len && response[msg_end] != '\0') msg_end++;

    if (msg_end >= 2 && response[msg_end - 2] == '\r' && response[msg_end - 1] == '\n') msg_end -= 2;
    copy_len = msg_end > i ? msg_end - i : 0;
    if (copy_len >= message_size) copy_len = message_size - 1;

    memcpy(message, response + i, copy_len);
    message[copy_len] = '\0';
}

static void _parse_ftp_request(const uint8_t *request, size_t pkt_len, char *command, size_t command_size, char *argument, size_t argument_size) {
    size_t i;
    size_t arg_len;
    
    i = 0;
    if (!request || pkt_len == 0 || !command || command_size == 0 || !argument || argument_size == 0) return;

    while (i < pkt_len && request[i] != ' ' && request[i] != '\0' && i < command_size - 1 && i < 4) {
        command[i] = (char)request[i];
        i ++;
    }
    command[i] = '\0';

    if (i < pkt_len && request[i] == ' ') {
        i ++;
        arg_len = pkt_len - i;

        if (arg_len >= 2 && request[i + arg_len - 2] == '\r' && request[i + arg_len - 1] == '\n') arg_len -= 2;
        if (arg_len >= argument_size) arg_len = argument_size - 1;

        memcpy(argument, &request[i], arg_len);
        argument[arg_len] = '\0';
    }
    else argument[0] = '\0';
}

static void _print_ftp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    size_t i;
    (void)pkt_len;
    (void)hdr_len;

    i = 0;
    if (!pkt || pkt[0] == '\0') return;
    
    if (isdigit(pkt[0])) printf("Response: ");
    else if (isupper(pkt[0])) printf("Request: ");

    while (pkt[i] != '\r') {
        printf("%c", pkt[i]);
        i ++;
    }
}

static void _visualize_ftp_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char str1[MAX_FTP_LINE_LEN];
    char str2[MAX_FTP_LINE_LEN];
    
    (void)pkt_len;
    (void)hdr_len;
    if (!pkt || pkt[0] == '\0') return;
    
    start_printing();
    if (isdigit(pkt[0])) {
        _parse_ftp_response(pkt, pkt_len, str1, sizeof(str1), str2, sizeof(str2));
        print_field(FTP_TYPE_LABEL, "Response", 0);
        if (strlen(str1) > 0) print_field(FTP_RES_CODE_LABEL, str1, 0);
        if (strlen(str2) > 0) print_field(FTP_TYPE_LABEL, str2, 0);
    }
    else if (isupper(pkt[0])) {
        _parse_ftp_request(pkt, pkt_len, str1, sizeof(str1), str2, sizeof(str2));
        print_field(FTP_TYPE_LABEL, "Response", 0);
        if (strlen(str1) > 0) print_field(FTP_COMMAND_LABEL, str1, 0);
        if (strlen(str2) > 0) print_field(FTP_ARGUMENT_LABEL, str2, 0);
    }
    end_printing();
}

protocol_info dissect_ftp(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    if (!pkt || pkt[0] == '\0') return NO_ENCAP_PROTO;

    SHOW_OUTPUT(pkt, pkt_len, 0, fmt, _print_ftp_hdr, _visualize_ftp_hdr);
    return NO_ENCAP_PROTO;
}