#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pcap.h>

#include "analize.h"
#include "../ethertypes.h"
#include "../utils/string_utils.h"
#include "../protocols/datalink/ether.h"
#include "../protocols/network/ip.h"
#include "../status_handler.h"

#include "../protocols/protocol_handler.h"
#include "../protocols/datalink_handler.h"

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

void dissect_packet(command *cmd, const uint8_t *pkt) {
	int show_datalink = NULL != get_arg(cmd, DATALINK_HDR_ARG);
	
	protocol_info datalink_info = dissect_datalink(pcap_datalink(handle));
	if (show_datalink) datalink_info.print_header(pkt);

	
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
