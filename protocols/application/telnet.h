#ifndef TELNET_H
#define TELNET_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

#define TELNET_COMMAND_LABEL        "Command"
#define TELNET_DATA_LABEL           "Data (', ' separator)"

#define TELNET_IAC                  0xff
#define TELNET_COMMAND_OFFSET       240  /* (command - TELNET_COMMAND_OFFSET) = correct array index */
#define TELNET_END_I_OPTIONS        49
#define TELNET_START_II_OPTIONS     138
#define TELNET_END_II_OPTIONS       140
#define TELNET_EXTENDED_OPT_LIST    255


protocol_info dissect_telnet(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif