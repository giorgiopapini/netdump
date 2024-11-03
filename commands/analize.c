#include <stdio.h>
#include <stdlib.h>
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

netdissect_info dissect_datalink(netdissect_info *datalink_info) {
	netdissect_info net_info;
	switch (datalink_info->protocol) {
		case DLT_EN10MB: {
			net_info.prev_layer_action = print_ether_hdr;
			net_info.protocol = ntohs(((struct ether_hdr *)datalink_info->pkt)->ethertype);
			net_info.pkt = datalink_info->pkt + sizeof(ether_hdr);
			break;
		}
		default: break;
	}
	return net_info;
}

netdissect_info dissect_network(netdissect_info *network_info) {
	netdissect_info transport_info;
	switch (network_info->protocol) {
		case ETHERTYPE_IP: {
			transport_info.prev_layer_action = print_ip_header;
			transport_info.protocol = ntohs(((struct ip_hdr *)network_info->pkt)->protocol);
			transport_info.pkt = network_info->pkt + sizeof(ip_hdr);
			break;
		};
		default: transport_info.prev_layer_action = NULL; break;
	}
	return transport_info;
}

/* IMPORTANT!!!!! --> Maybe using lookup table instead of switch cases? Eventually make a module for each dissection function */

void dissect_packet(command *cmd, const uint8_t *pkt) {
	int show_datalink = NULL == get_arg(cmd, DATALINK_HDR_ARG) ? 0 : 1;
	int show_network = 1;
	netdissect_info datalink_info = { .protocol = pcap_datalink(handle), .pkt = pkt };

	netdissect_info network_info = dissect_datalink(&datalink_info);
	if (show_datalink && NULL != network_info.prev_layer_action) network_info.prev_layer_action(datalink_info.pkt);

	netdissect_info transport_info = dissect_network(&network_info);  // pkt + correct offset based on datalink protocol
	if (show_network && NULL != transport_info.prev_layer_action) transport_info.prev_layer_action(network_info.pkt); 
}

void get_packet(uint8_t *args, const struct pcap_pkthdr *header, const uint8_t *pkt) {
	custom_data *data = (custom_data *)args;
	size_t total_bytes = header->len;

	void *dynamic_pkt = (void *)malloc(total_bytes);
	memcpy(dynamic_pkt, pkt, total_bytes);
	insert(data->packets, dynamic_pkt);

	dissect_packet(data->cmd, pkt);
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

	if (-1 == pcap_datalink(handle)) raise_error(DATALINK_HEADER_ERROR, 1, NULL, pcap_geterr(handle));
	
	signal(SIGINT, handle_sigint);

	printf("device: %s\n", dev);
	if (-1 == pcap_compile(handle, &fp, filter_exp, 0, net)) raise_error(INVALID_FILTER, 0, NULL, filter_exp);
	else if (-1 == pcap_setfilter(handle, &fp)) raise_error(NOT_INTALLABLE_FILTER, 0, NULL, filter_exp);
	else if (-1 == pcap_loop(handle, pkt_num, get_packet, (uint8_t *)&custom_args)) raise_error(PCAP_LOOP_ERROR, 1, NULL);
	else printf("\ntotal packets: %d\n", packets->len);

	pcap_close(handle);
}
