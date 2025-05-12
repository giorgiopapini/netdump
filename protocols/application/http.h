#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <stdint.h>

#include "../../utils/protocol.h"

#define HTTP_HEADER_LABEL       "HTTP Header"
#define HTTP_METHOD_LABEL       "Method"
#define HTTP_PATH_LABEL         "Path"
#define HTTP_VERSION_LABEL      "Version"

#define MAX_HTTP_HDR_LEN        1024


protocol_info dissect_http(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif