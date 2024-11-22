#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <pcap.h>

typedef struct packet {
    struct pcap_pkthdr *header;
    int datalink_type;
    uint8_t *bytes;
} packet;

packet *create_packet(struct pcap_pkthdr *hdr, int datalink_type, uint8_t *bytes);
void destroy_packet(void *pkt);

#endif