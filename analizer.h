#ifndef ANALIZER_H
#define ANALIZER_H

#include <pcap.h>
#include "utils/raw_array.h"

void get_packet(__u_char *args, const struct pcap_pkthdr *header, const __u_char *packet);
void sniff_packets(raw_array *packets, int n, char *filter_exp);  /* int n = amount of packets to scan; (-1) means endless retrieve */

#endif