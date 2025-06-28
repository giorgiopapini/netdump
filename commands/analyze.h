#ifndef ANALYZE_H
#define ANALYZE_H

#include <stdint.h>

#include "../utils/shared_lib.h"
#include "../utils/command.h"
#include "../utils/raw_array.h"
#include "../utils/custom_dissectors.h"
#include "../utils/hierachy.h"

struct pcap_pkthdr;


#define CAPTURE_FROM_FILE_MSG "Reading from pcap file: %s\n"
#define CAPTURE_DEVICE_MSG "Capturing from live device: %s\n"


int device_exists(char *dev);
void get_packet(uint8_t *args, const struct pcap_pkthdr *header, const uint8_t *bytes);
void execute_analyze(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_diss, hierarchy_node *root);

#endif