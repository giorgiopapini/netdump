#ifndef ANALIZE_H
#define ANALIZE_H

#include <stdint.h>
#include "../command_handler.h"

void execute_analize(command *cmd, raw_array *packets);
void print_link_level_header(int datalink_type, const uint8_t *pkt);

#endif