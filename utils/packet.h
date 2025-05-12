#ifndef PACKET_H
#define PACKET_H

#include <stddef.h>
#include <stdint.h>


struct pcap_pkthdr;

typedef struct packet {
    struct pcap_pkthdr *header;
    int datalink_type;
    size_t num;
    uint8_t *bytes;
} packet;

packet *create_packet(const struct pcap_pkthdr *hdr, int datalink_type, size_t num, const uint8_t *bytes);
void destroy_packet(void *pkt);

#endif