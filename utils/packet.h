#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <pcap.h>

typedef struct packet {
    struct pcap_pkthdr *header;
    int datalink_type;
    size_t num;
    uint8_t *bytes;
} packet;

packet *create_packet(const struct pcap_pkthdr *hdr, int datalink_type, size_t num, const uint8_t *bytes);
void destroy_packet(void *pkt);

#endif