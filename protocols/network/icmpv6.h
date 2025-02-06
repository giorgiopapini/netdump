#ifndef ICMPV6_H
#define ICMPV6_H

#include <stdint.h>
#include "../../protocol_handler.h"


protocol_info dissect_icmpv6(const uint8_t *pkt, const char *proto_name, output_format fmt);

#endif