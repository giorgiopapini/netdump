#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pcap.h>

#include "analizer.h"
#include "ethertypes.h"
#include "protocols/ether.h"
#include "protocols/ip.h"
#include "status_handler.h"

#define LOOP_TO_INFINITY -1


void get_packet(__u_char *args, const struct pcap_pkthdr *header, const __u_char *pkt) {
	raw_array *packets = (raw_array *)args;
	size_t total_bytes = header->len;

	void *dynamic_pkt = (void *)malloc(total_bytes);
	memcpy(dynamic_pkt, pkt, total_bytes);
	insert(packets, dynamic_pkt);
	
	struct ether_hdr *eth_h;
	struct ip_hdr *ip_h;
	eth_h = (struct ether_hdr *) (pkt);

	/*
	printf("src:");
	for (int i = 0; i < 6; i ++) {
		printf("%02x ", eth_header->src_addr[i] & 0xff);
	}
	printf(" dest:");
	for (int i = 0; i < 6; i ++) {
		printf("%02x ", eth_header->dest_addr[i] & 0xff);
	}
	printf(" ethertype: %d", ntohs(eth_header->ethertype));
	printf("\n");
	*/
	if (ntohs(eth_h->ethertype) == ETHERTYPE_IP) {
		ip_h = (struct ip_hdr *)(pkt + sizeof(ether_hdr));
		
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

}

// https://vichargrave.github.io/programming/develop-a-packet-sniffer-with-libpcap/

// Around line 500 and after ==> Switch chase with every packet supported for decoding
// https://github.com/the-tcpdump-group/tcpdump/blob/d44658b2e47ad1a3724a632f0d65a81140654c15/print-ether.c#L541

void sniff_packets(raw_array *packets, int n, char *filter_exp) {
	pcap_t *handle;
	char *dev;
	char errbuff[PCAP_ERRBUF_SIZE];
	int datalink_type;
	struct bpf_program fp;
	bpf_u_int32 mask;
	bpf_u_int32 net;

	dev = pcap_lookupdev(errbuff);
	if (NULL == dev) {
		raise(NO_DEVICE_FOUND, 1, NULL);
		mask = 0;
		net = 0;
	}

	if (-1 == pcap_lookupnet(dev, &net, &mask, errbuff)) raise(NETMASK_ERROR, 1, NULL, dev);

	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuff);		// promiscuos mode (third argument) = 1
	if (NULL == handle) raise(NO_ACCESS_DEVICE_ERROR, 1, NULL, dev);

	datalink_type = pcap_datalink(handle);
	if (-1 == datalink_type) raise(DATALINK_HEADER_ERROR, 1, NULL, pcap_geterr(handle));
	
	printf("\ndevice: %s\n", dev);

	if (-1 == pcap_compile(handle, &fp, filter_exp, 0, net)) raise(INVALID_FILTER, 1, NULL, filter_exp);
	if (-1 == pcap_setfilter(handle, &fp)) raise(NOT_INTALLABLE_FILTER, 1, NULL, filter_exp);
	if (-1 == pcap_loop(handle, n, get_packet, (__u_char *)packets)) raise(PCAP_LOOP_ERROR, 1, NULL);

	pcap_close(handle);
}