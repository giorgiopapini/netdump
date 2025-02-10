#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ftp.h"
#include "../../utils/visualizer.h"


void parse_ftp_response(const uint8_t *response, char *code, char *message) {
    size_t len;
    if (!response || strlen(response) < 5) return;

    strncpy(code, response, 3);
    code[3] = '\0';
    strcpy(message, response + 4);
    
    len = strlen(message);
    if (len >= 2 && message[len - 2] == '\r' && message[len - 1] == '\n') message[len - 2] = '\0';
}

void parse_ftp_request(const char *request, char *command, char *argument) {
    if (!request || strlen(request) < 1) return;

    size_t i = 0;
    while (request[i] != ' ' && request[i] != '\0' && i < 4) {
        command[i] = request[i];
        i++;
    }
    command[i] = '\0';

    if (request[i] == ' ') {
        strcpy(argument, request + i + 1);
        
        size_t len = strlen(argument);
        if (len >= 2 && argument[len - 2] == '\r' && argument[len - 1] == '\n') argument[len - 2] = '\0';
    } 
    else argument[0] = '\0';
}

void print_ftp_hdr(const uint8_t *pkt) {
    int i = 0;
    if (!pkt || pkt[0] == '\0') return;
    
    if (isdigit(pkt[0])) printf("Response: ");
    else if (isupper(pkt[0])) printf("Request: ");

    while (pkt[i] != '\r') {
        printf("%c", pkt[i]);
        i ++;
    }
}

void visualize_ftp_hdr(const uint8_t *pkt) {
    char str1[FTP_HEADER_MAX_LEN];
    char str2[FTP_HEADER_MAX_LEN];
    if (!pkt || pkt[0] == '\0') return;
    
    start_printing();
    print_hdr_info(FTP_HEADER_LABEL, NULL);

    if (isdigit(pkt[0])) {
        parse_ftp_response(pkt, str1, str2);
        print_field(FTP_TYPE_LABEL, "Response", 0);
        if (strlen(str1) > 0) print_field(FTP_RES_CODE_LABEL, str1, 0);
        if (strlen(str2) > 0) print_field(FTP_TYPE_LABEL, str2, 0);
    }
    else if (isupper(pkt[0])) {
        parse_ftp_request(pkt, str1, str2);
        print_field(FTP_TYPE_LABEL, "Response", 0);
        if (strlen(str1) > 0) print_field(FTP_COMMAND_LABEL, str1, 0);
        if (strlen(str2) > 0) print_field(FTP_ARGUMENT_LABEL, str2, 0);
    }
    end_printing();
}

protocol_info dissect_ftp(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    SHOW_OUTPUT(pkt, fmt, proto_name, print_ftp_hdr, visualize_ftp_hdr);
    return NO_ENCAP_PROTO;
}