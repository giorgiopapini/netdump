#ifndef FTP_H
#define FTP_H

#include <stddef.h>
#include <stdint.h>
#include "../../utils/protocol.h"

#define MAX_FTP_HDR_LEN_LABEL        "FTP Header"
#define FTP_TYPE_LABEL          "Type"
#define FTP_RES_CODE_LABEL      "Code"
#define FTP_COMMAND_LABEL       "Command"
#define FTP_MESSAGE_LABEL       "Message"
#define FTP_ARGUMENT_LABEL      "Arguments"

#define MAX_FTP_HDR_LEN         512


protocol_info dissect_ftp(const uint8_t *pkt, size_t pkt_len, output_format fmt);

#endif