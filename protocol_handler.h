#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include "utils/command.h"
#include "utils/packet.h"
#include "utils/custom_dissectors.h"
#include "utils/shared_lib.h"


void dissect_packet(command *cmd, packet *pkt, shared_libs *libs, custom_dissectors *custom_dissectors);

#endif