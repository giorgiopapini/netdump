#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <pcap.h>

typedef struct packet {
    struct pcap_pkthdr *header;
    uint8_t *bytes;
} packet;

packet *create_packet(struct pcap_pkthdr *hdr, uint8_t *bytes);
void destroy_packet(void *pkt);

#endif