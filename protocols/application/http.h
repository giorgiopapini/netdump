#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <stdint.h>

#include "../../utils/protocol.h"

#define HTTP_METHOD_LABEL       "Method"
#define HTTP_PATH_LABEL         "Path"
#define HTTP_VERSION_LABEL      "Version"

#define MAX_LINE_LEN            4096
#define MAX_KEY_LEN             64
#define MAX_VALUE_LEN           4096
#define MAX_METHOD_LEN          16
#define MAX_PATH_LEN            1024
#define MAX_VERSION_LEN         16


protocol_info dissect_http(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif