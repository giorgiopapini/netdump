#include "protocol_handler.h"
#include "../status_handler.h"

#include <arpa/inet.h>


int get_field(const uint8_t *pkt, field byte_segment) {
    int i;
    int val = 0;

    if (NULL == pkt) raise_error(NULL_POINTER, 1, NULL, "pkt", __FILE__);

    if (1 == byte_segment.len) return pkt[byte_segment.start];
    for (i = 0; i < byte_segment.len; i ++) {
        val |= pkt[byte_segment.start + i] << (8 * i);
    }

    if (0 != byte_segment.ntoh) {
        if (2 == byte_segment.len) return ntohs(val);
        if (4 == byte_segment.len) return ntohl(val);
    }
    return val;
}