#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "http.h"
#include "../../utils/visualizer.h"


void extract_http_request_line(const char *request_line, char *method, char *path, char version[HTTP_MAX_HEADER_LEN]);
void split_header_line(const char *header_line, char *key, char value[HTTP_MAX_HEADER_LEN]);
void print_http_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_http_hdr(const uint8_t *pkt, size_t pkt_len);

void extract_http_request_line(const char *request_line, char *method, char *path, char version[HTTP_MAX_HEADER_LEN]) {
    ptrdiff_t path_len;
    const char *path_start;
    const char *path_end;
    const char *version_start;

    path_start = strchr(request_line, ' ') + 1;
    path_end = strchr(path_start, ' ');
    version_start = NULL;

    sscanf(request_line, "%s", method);

    if (path_end != NULL) {
        path_len = path_end - path_start;

        if (path_len > 0) {
            strncpy(path, path_start, (size_t)path_len);  /* path_len is > 0, it can be casted safely */
            path[path_len] = '\0';
        }
        else path[0] = '\0';

        version_start = strchr(path_end, ' ');
    }

    if (version_start != NULL) {
        strncpy(version, version_start + 1, HTTP_MAX_HEADER_LEN - 1);
        version[HTTP_MAX_HEADER_LEN - 1] = '\0';
    }
    else version[0] = '\0';
}

void split_header_line(const char *header_line, char *key, char value[HTTP_MAX_HEADER_LEN]) {
    ptrdiff_t key_len;
    const char *value_start;
    const char *colon_pos;
    colon_pos = strchr(header_line, ':');
    
    if (colon_pos != NULL) {
        key_len = colon_pos - header_line;
        if (key_len > 0) {
            strncpy(key, header_line, (size_t)key_len);
            key[(size_t)key_len] = '\0';

            value_start = colon_pos + 1;
            while (*value_start == ' ') value_start ++;

            if (value_start != NULL) {
                strncpy(value, value_start + 1, HTTP_MAX_HEADER_LEN - 1);
                value[HTTP_MAX_HEADER_LEN - 1] = '\0';
            }
        }
    } 
    else {
        key[0] = '\0';
        value[0] = '\0';
    }
}

void print_http_hdr(const uint8_t *pkt, size_t pkt_len) {
    size_t i;
    for (i = 0; i < pkt_len; i ++) printf("%c", pkt[i]);
}

void visualize_http_hdr(const uint8_t *pkt, size_t pkt_len) {
    char line[HTTP_MAX_HEADER_LEN];
    char key[HTTP_MAX_HEADER_LEN];
    char value[HTTP_MAX_HEADER_LEN];
    char method[HTTP_MAX_HEADER_LEN];
    char path[HTTP_MAX_HEADER_LEN];
    char version[HTTP_MAX_HEADER_LEN];

    const uint8_t *ptr = pkt;
    const uint8_t *end = pkt + pkt_len;
    size_t len;

    len = strcspn((const char *)ptr, "\r\n");
    if (len >= sizeof(line)) len = sizeof(line) - 1;
    strncpy(line, (const char *)ptr, len);
    line[len] = '\0';

    ptr += len;
    if (ptr < end && *ptr == '\r') ptr ++;
    if (ptr < end && *ptr == '\n') ptr ++;

    start_printing();
    print_additional_info("Payload not represented in ascii art");

    extract_http_request_line(line, method, path, version);
    print_field(HTTP_METHOD_LABEL, method, 0);
    print_field(HTTP_PATH_LABEL, path, 0);
    print_field(HTTP_VERSION_LABEL, version, 0);

    while (ptr < end) {
        len = strcspn((const char *)ptr, "\r\n");
        if (len >= sizeof(line)) len = sizeof(line) - 1;
        strncpy(line, (const char *)ptr, len);
        line[len] = '\0';

        ptr += len;
        if (ptr < end && *ptr == '\r') ptr ++;
        if (ptr < end && *ptr == '\n') ptr ++;

        if (line[0] == '\0') break;

        split_header_line(line, key, value);
        if (key[0] != '\0') print_field(key, value, 0);
    }
    end_printing();
}

protocol_info dissect_http(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_http_hdr, visualize_http_hdr);
    return NO_ENCAP_PROTO;
}