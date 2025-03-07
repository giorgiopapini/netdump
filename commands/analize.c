#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pcap.h>

#include "analize.h"
#include "../utils/packet.h"
#include "../utils/string_utils.h"
#include "../status_handler.h"

#include "../protocol_handler.h"

#define LOOP_TO_INFINITY -1

static pcap_t *handle;


typedef struct custom_data {
	command *cmd;
	raw_array *packets;
	pcap_dumper_t *pcap_dump;
} custom_data;

void handle_sigint(int sig) {
	pcap_breakloop(handle);
	printf("\n");
}

int device_exists(char *dev) {
	pcap_if_t *alldevs;
    pcap_if_t *device;
    char errbuff[PCAP_ERRBUF_SIZE];

    if (-1 == pcap_findalldevs(&alldevs, errbuff)) {
        raise_error(DEVICES_SCAN_ERROR, 0, NULL);
        return 0;
    }

    for (device = alldevs; NULL != device; device = device->next) {
        if (0 == strcmp(device->name, dev)) {
			pcap_freealldevs(alldevs);
			return 1;
		}
    }
    pcap_freealldevs(alldevs);

	return 0;
}

void get_packet(uint8_t *args, const struct pcap_pkthdr *header, const uint8_t *bytes) {
	custom_data *data = (custom_data *)args;

	/* data->packets->len + 1 because otherwise it would start from 0 */
	packet *pkt = create_packet(header, pcap_datalink(handle), data->packets->len + 1, bytes);
	insert(data->packets, pkt);
	
	if (NULL != data->pcap_dump) pcap_dump((uint8_t *)data->pcap_dump, header, bytes);

	dissect_packet(data->cmd, pkt);
}

void execute_analize(command *cmd, raw_array *packets) {
	/* ==============================  Getting args values from cmd  ================================ */
	int pkt_num = -1;
    int tmp = str_to_num(get_raw_val(cmd, NUMBER_ARG));
    char *filter_exp = get_raw_val(cmd, FILTER_ARG);
	char *dev = get_raw_val(cmd, DEVICE_ARG);
	char *read_file = get_raw_val(cmd, READ_FILE_ARG);
	char *write_file = get_raw_val(cmd, WRITE_FILE_ARG);
    int prom_mode = NULL == get_arg(cmd, NO_PROM_ARG);

    /* if -n not provided (or -n value not set) returns 0. Analizing 0 packets doesn't make sense, so assume to scan to infinity */
    if (0 != tmp) pkt_num = tmp;
	/* ============================================================================================== */

    custom_data custom_args = { .cmd = cmd, .packets = packets, .pcap_dump = NULL };
	char errbuff[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;
	bpf_u_int32 mask;
	bpf_u_int32 net;

	if (NULL != read_file) {
        handle = pcap_open_offline(read_file, errbuff);
        if (NULL == handle) {
			raise_error(PCAP_FILE_ERROR, 0, ABSOLUTE_PATH_HINT, read_file);
			return;
		}
		printf(CAPTURE_FROM_FILE_MSG, read_file);
    } else {
		if (NULL == dev) dev = pcap_lookupdev(errbuff);
        if (NULL == dev) {
            raise_error(NO_DEVICE_FOUND, 1, NO_DEVICE_HINT, dev);
            mask = 0;
            net = 0;
        }

		if (!device_exists(dev)) {
			raise_error(NO_DEVICE_FOUND, 0, NO_DEVICE_HINT, dev);
			return;
		}

        if (-1 == pcap_lookupnet(dev, &net, &mask, errbuff)) {
			raise_error(NETMASK_ERROR, 0, NULL, dev);
			return;
		}

        handle = pcap_open_live(dev, BUFSIZ, prom_mode, 1000, errbuff);  // promiscuous mode (third argument) = 1
        if (NULL == handle) raise_error(NO_ACCESS_DEVICE_ERROR, 1, NULL, dev);

        printf(CAPTURE_DEVICE_MSG, dev);
    }

	if (-1 == pcap_datalink(handle)) raise_error(DATALINK_HEADER_ERROR, 1, NULL, pcap_geterr(handle));
	
	signal(SIGINT, handle_sigint);

	if (NULL != write_file) {
		custom_args.pcap_dump = pcap_dump_open(handle, write_file);
		if (NULL == custom_args.pcap_dump) {
			pcap_close(handle);
			raise_error(PCAP_FILE_ERROR, 0, ABSOLUTE_PATH_HINT, write_file);
			return;
		}
	}

	if (-1 == pcap_compile(handle, &fp, filter_exp, 0, net)) raise_error(INVALID_FILTER, 0, NULL, filter_exp);
	else if (-1 == pcap_setfilter(handle, &fp)) raise_error(NOT_INTALLABLE_FILTER, 0, NULL, filter_exp);
	else if (-1 == pcap_loop(handle, pkt_num, get_packet, (uint8_t *)&custom_args)) raise_error(PCAP_LOOP_ERROR, 1, NULL);
	else printf("\ntotal packets: %d\n", packets->len);

	if (NULL != custom_args.pcap_dump) pcap_dump_close(custom_args.pcap_dump);
	pcap_close(handle);
}
