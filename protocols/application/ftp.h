#ifndef FTP_H
#define FTP_H

#include <stdint.h>
#include "../../protocol_handler.h"

#define FTP_HEADER_LABEL        "FTP Header"
#define FTP_TYPE_LABEL          "Type"
#define FTP_RES_CODE_LABEL      "Code"
#define FTP_COMMAND_LABEL       "Command"
#define FTP_MESSAGE_LABEL       "Message"
#define FTP_ARGUMENT_LABEL      "Arguments"

#define FTP_HEADER_MAX_LEN      512


protocol_info dissect_ftp(const uint8_t *pkt, uint32_t pkt_len, output_format fmt);

#endif