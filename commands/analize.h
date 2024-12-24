#ifndef ANALIZE_H
#define ANALIZE_H

#include <stdint.h>
#include "../command_handler.h"

#define CAPTURE_FROM_FILE_MSG "Reading from pcap file: %s\n"
#define CAPTURE_DEVICE_MSG "Capturing from live device: %s\n"

void execute_analize(command *cmd, raw_array *packets);

#endif