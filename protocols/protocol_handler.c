#include "protocol_handler.h"
#include "../status_handler.h"

int get_field(const uint8_t *pkt, field encap_type_range) {
    int i;
    int val = 0;

    if (NULL == pkt) raise_error(NULL_POINTER, 1, NULL, "pkt", __FILE__);

    if (1 == encap_type_range.len) return pkt[encap_type_range.start];
    for (i = 0; i < encap_type_range.len; i ++) {
        val |= pkt[encap_type_range.start + i] << (8 * i);
    }
    return val;
}