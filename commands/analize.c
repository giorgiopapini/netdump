#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <pcap.h>

#include "analize.h"
#include "../ethertypes.h"
#include "../utils/string_utils.h"
#include "../protocols/ether.h"
#include "../protocols/ip.h"
#include "../status_handler.h"

#define LOOP_TO_INFINITY -1

static pcap_t *handle;

typedef struct custom_data {
	command *cmd;
	raw_array *packets;
} custom_data;

void handle_sigint(int sig) {
	pcap_breakloop(handle);
	printf("\n");
}

void get_packet(uint8_t *args, const struct pcap_pkthdr *header, const uint8_t *pkt) {
	custom_data *data = (custom_data *)args;
	raw_array *packets = data->packets;
	command *cmd = data->cmd;

	size_t total_bytes = header->len;

	void *dynamic_pkt = (void *)malloc(total_bytes);
	memcpy(dynamic_pkt, pkt, total_bytes);
	insert(packets, dynamic_pkt);
	
	struct ether_hdr *eth_h;
	struct ip_hdr *ip_h;
	eth_h = (struct ether_hdr *) (pkt);

    if (get_arg(cmd, ETHER_HEADER_ARG))	print_ether_hdr(pkt);
	
	
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

void execute_analize(command *cmd, raw_array *packets) {
	/* ==============================  Getting args values from cmd  ================================ */
	int pkt_num = -1;
    int tmp = str_to_num(get_raw_val(cmd, QUANTITY_ARG));
    char *filter_exp = get_raw_val(cmd, FILTER_ARG);
    int prom_mode = NULL == get_arg(cmd, NO_PROM_ARG);

    /* if -n not provided (or -n value not set) returns 0. Analizing 0 packets doesn't make sense, so assume to scan to infinity */
    if (0 != tmp) pkt_num = tmp;
	/* ============================================================================================== */

    custom_data custom_args = { .cmd = cmd, .packets = packets };
	char *dev;
	char errbuff[PCAP_ERRBUF_SIZE];
	int datalink_type;
	struct bpf_program fp;
	bpf_u_int32 mask;
	bpf_u_int32 net;

	dev = pcap_lookupdev(errbuff);
	if (NULL == dev) {
		raise_error(NO_DEVICE_FOUND, 1, NULL);
		mask = 0;
		net = 0;
	}

	if (-1 == pcap_lookupnet(dev, &net, &mask, errbuff)) raise_error(NETMASK_ERROR, 1, NULL, dev);

	handle = pcap_open_live(dev, BUFSIZ, prom_mode, 1000, errbuff);		// promiscuos mode (third argument) = 1
	if (NULL == handle) raise_error(NO_ACCESS_DEVICE_ERROR, 1, NULL, dev);

	datalink_type = pcap_datalink(handle);
	if (-1 == datalink_type) raise_error(DATALINK_HEADER_ERROR, 1, NULL, pcap_geterr(handle));
	
	signal(SIGINT, handle_sigint);

	printf("device: %s\n", dev);
	if (-1 == pcap_compile(handle, &fp, filter_exp, 0, net)) raise_error(INVALID_FILTER, 0, NULL, filter_exp);
	else if (-1 == pcap_setfilter(handle, &fp)) raise_error(NOT_INTALLABLE_FILTER, 0, NULL, filter_exp);
	else if (-1 == pcap_loop(handle, pkt_num, get_packet, (uint8_t *)&custom_args)) raise_error(PCAP_LOOP_ERROR, 1, NULL);
	else printf("\ntotal packets: %d\n", packets->len);

	pcap_close(handle);
}
