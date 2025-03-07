#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include "utils/command.h"
#include "utils/packet.h"


void dissect_packet(command *cmd, packet *pkt);

#endif