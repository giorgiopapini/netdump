#include <stdio.h>
#include <arpa/inet.h>

#include "ip.h"

void print_ip_header(const uint8_t *pkt) {
    ip_hdr *ip_h = (struct ip_hdr *)pkt;
		
    printf("\nsrc: ");
    printf("%u", ntohl(ip_h->src_addr));
    printf("\n");

    printf("dest: ");
    printf("%u", ntohl(ip_h->dest_addr));	
    printf("\n");

    printf("size: ");
    printf("%d", sizeof(pkt));
    printf("\n");
}