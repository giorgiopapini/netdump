#ifndef ANALIZE_H
#define ANALIZE_H

#include <stdint.h>
#include <pcap.h>

#include "../command_handler.h"
#include "../utils/custom_dissectors.h"
#include "../utils/shared_lib.h"

#define CAPTURE_FROM_FILE_MSG "Reading from pcap file: %s\n"
#define CAPTURE_DEVICE_MSG "Capturing from live device: %s\n"


void handle_sigint(int sig);
int device_exists(char *dev);
void get_packet(uint8_t *args, const struct pcap_pkthdr *header, const uint8_t *bytes);
void execute_analize(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_dissectors);

#endif