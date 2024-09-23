#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pcap.h>

#include "analizer.h"
#include "ethertypes.h"
#include "protocols/ether.h"
#include "protocols/ip.h"

#define LOOP_TO_INFINITY -1


void get_packet(__u_char *args, const struct pcap_pkthdr *header, const __u_char *pkt) {
	raw_array *packets = (raw_array *)args;
	size_t total_bytes = header->len;

	void *dynamic_pkt = (void *)malloc(total_bytes);
	memcpy(dynamic_pkt, pkt, total_bytes);
	insert(packets, dynamic_pkt);
	
	struct ether_header *eth_h;
	struct ip_header *ip_h;
	eth_h = (struct ether_header *) (pkt);

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
		ip_h = (struct ip_header *)(pkt + sizeof(ether_header));
		
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

void sniff_packets(raw_array *packets, int n) {
	pcap_t *handle;
	char *dev;
	char errbuff[PCAP_ERRBUF_SIZE];
	int datalink_type;
	struct bpf_program fp;
	char filter_exp[] = "";
	bpf_u_int32 mask;
	bpf_u_int32 net;

	dev = pcap_lookupdev(errbuff);
	if (NULL == dev) {
		fprintf(stderr, "Couldn't find default device: %s\n", errbuff);
		mask = 0;
		net = 0;
	}

	if (-1 == pcap_lookupnet(dev, &net, &mask, errbuff)) {
		fprintf(stderr, "Couldn't get netmask for device: %s\n", dev);
		exit(2);
	}

	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuff);		// promiscuos mode (third argument) = 1
	if (NULL == handle) {
		fprintf(stderr, "Couldn't open device: %s\n", dev);
		exit(2);
	}

	datalink_type = pcap_datalink(handle);
	if (-1 == datalink_type) {
		fprintf(stderr, "Could't get datalink header type: %s\n", pcap_geterr(handle));
		exit(2);
	}
	printf("\ndevice: %s\n", dev);
	printf("data: %d\n", datalink_type);

	if (-1 == pcap_compile(handle, &fp, filter_exp, 0, net)) {
		fprintf(stderr, "Could't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(2);
	}

	if (-1 == pcap_setfilter(handle, &fp)) {
		fprintf(stderr, "Couldn't install filter: %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(2);
	}

	if (-1 == pcap_loop(handle, n, get_packet, (__u_char *)packets)) {
		fprintf(stderr, "pcap_loop() failed\n");
		exit(2);
	}

	pcap_close(handle);
}