#include "http.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "../../utils/visualizer.h"
#include "../../utils/protocol.h"


int extract_http_request_line(const char *request_line, char *method, char *path, char version[MAX_HTTP_HDR_LEN]);
void split_header_line(const char *header_line, char *key, char value[MAX_HTTP_HDR_LEN]);
void print_http_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_http_hdr(const uint8_t *pkt, size_t pkt_len);

int extract_http_request_line(const char *request_line, char *method, char *path, char version[MAX_HTTP_HDR_LEN]) {
    ptrdiff_t path_len;
    char *first_space;
    const char *path_start;
    const char *path_end;
    const char *version_start;

    first_space = strchr(request_line, ' ');
    if (NULL == first_space) return 0;

    path_start = first_space + 1;

    path_end = strchr(path_start, ' ');
    if (NULL == path_end) return 0;

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
        strncpy(version, version_start + 1, MAX_HTTP_HDR_LEN - 1);
        version[MAX_HTTP_HDR_LEN - 1] = '\0';
    }
    else version[0] = '\0';
    return 1;
}

void split_header_line(const char *header_line, char *key, char value[MAX_HTTP_HDR_LEN]) {
    ptrdiff_t key_len;
    const char *value_start;
    const char *colon_pos;
    colon_pos = strchr(header_line, ':');
    
    if (colon_pos != NULL) {
        key_len = colon_pos - header_line;
        if (key_len > 0) {
            strncpy(key, header_line, (size_t)key_len);
            key[(size_t)key_len] = '\0';

            value_start = colon_pos;
            while (*value_start == ' ') value_start ++;

            if (value_start != NULL) {
                strncpy(value, value_start + 1, MAX_HTTP_HDR_LEN - 1);
                value[MAX_HTTP_HDR_LEN - 1] = '\0';
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
    char line[MAX_HTTP_HDR_LEN];
    char key[MAX_HTTP_HDR_LEN];
    char value[MAX_HTTP_HDR_LEN];
    char method[MAX_HTTP_HDR_LEN];
    char path[MAX_HTTP_HDR_LEN];
    char version[MAX_HTTP_HDR_LEN];

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


    if (0 == extract_http_request_line(line, method, path, version)) return;

    start_printing();
    print_additional_info("Payload not represented in ascii art");

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