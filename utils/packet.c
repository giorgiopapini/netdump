#include "packet.h"

#include <pcap/pcap.h>
#include <stdlib.h>
#include <string.h>

#include "../status_handler.h"


packet *create_packet(const struct pcap_pkthdr *hdr, int datalink_type, size_t num, const uint8_t *bytes) {
    packet *new_pkt;
    struct pcap_pkthdr *new_hdr;
    void *new_bytes;

    /* dinamically allocate a new struct pcap_pkthdr */
    new_hdr = malloc(sizeof *new_hdr);
    CHECK_NULL_EXIT(new_hdr);
    CHECK_NULL_EXIT(hdr);

    new_hdr->caplen = hdr->caplen;
    new_hdr->len = hdr->len;
    new_hdr->ts = hdr->ts;

    /* dinamically allocate a copy of uint8_t *bytes string */
    new_bytes = malloc(hdr->len);
    CHECK_NULL_EXIT(new_bytes);

	memcpy(new_bytes, bytes, hdr->len);

    /* dinamically allocate a new packet and populate members with freshly allocated header and bytes string */
    new_pkt = malloc(sizeof *new_pkt);
    CHECK_NULL_EXIT(new_pkt);

    new_pkt->header = new_hdr;
    new_pkt->datalink_type = datalink_type;
    new_pkt->num = num;
    new_pkt->bytes = new_bytes;
    return new_pkt;
}

void destroy_packet(void *pkt) {
    CHECK_NULL_RET(pkt);
    free(((packet *)pkt)->header);
    free(((packet *)pkt)->bytes);
    free((packet *)pkt);
}