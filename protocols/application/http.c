#include <stdio.h>
#include <string.h>

#include "http.h"
#include "../../utils/visualizer.h"


uint32_t http_len;

void extract_http_request_line(const char *request_line, char *method, char *path, char *version) {
    sscanf(request_line, "%s", method);

    const char *path_start = strchr(request_line, ' ') + 1;
    const char *path_end = strchr(path_start, ' ');

    if (path_end != NULL) {
        size_t path_len = path_end - path_start;
        strncpy(path, path_start, path_len);
        path[path_len] = '\0';
    } 
    else path[0] = '\0';

    const char *version_start = path_end + 1;
    strcpy(version, version_start);
}

void split_header_line(const char *header_line, char *key, char *value) {
    const char *colon_pos = strchr(header_line, ':');
    
    if (colon_pos != NULL) {
        size_t key_len = colon_pos - header_line;
        strncpy(key, header_line, key_len);
        key[key_len] = '\0';

        const char *value_start = colon_pos + 1;
        while (*value_start == ' ') value_start ++;

        strcpy(value, value_start);
    } 
    else {
        key[0] = '\0';
        value[0] = '\0';
    }
}

void print_http_hdr(const uint8_t *pkt) {
    int i;
    for (i = 0; i < http_len; i ++) printf("%c", pkt[i]);
}

void visualize_http_hdr(const uint8_t *pkt) {
    char line[HTTP_MAX_HEADER_LEN];
    char key[HTTP_MAX_HEADER_LEN];
    char value[HTTP_MAX_HEADER_LEN];
    char method[HTTP_MAX_HEADER_LEN];
    char path[HTTP_MAX_HEADER_LEN];
    char version[HTTP_MAX_HEADER_LEN];

    const uint8_t *ptr = pkt;

    sscanf(ptr, "%[^\r\n]", line);  /* Read the first line (request line) */
    ptr += strlen(line) + 2;  /* Skip over the first line's "\r\n" */

    start_printing();
    print_hdr_info(HTTP_HEADER_LABEL, "Payload not represented in ascii art");

    extract_http_request_line(line, method, path, version);
    print_field(HTTP_METHOD_LABEL, method, 0);
    print_field(HTTP_PATH_LABEL, path, 0);
    print_field(HTTP_VERSION_LABEL, version, 0);

    while (sscanf(ptr, "%[^\r\n]", line) > 0) {
        ptr += strlen(line) + 2;  /* +2 to skip over \r\n */
        split_header_line(line, key, value);

        if (key[0] != '\0') print_field(key, value, 0);
    }
    end_printing();
}

protocol_info dissect_http(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    if (pkt_len > 0) {
        http_len = pkt_len;
        SHOW_OUTPUT(pkt, fmt, proto_name, print_http_hdr, visualize_http_hdr);
    }
    return NO_ENCAP_PROTO;
}