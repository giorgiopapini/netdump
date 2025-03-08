#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include "utils/command.h"
#include "utils/packet.h"
#include "utils/custom_dissectors.h"


void dissect_packet(command *cmd, packet *pkt, custom_dissectors *custom_dissectors);

#endif