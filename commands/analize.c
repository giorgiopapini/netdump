#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

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
	shared_libs *libs;
	custom_dissectors *custom_dissectors;
	pcap_dumper_t *pcap_dump;
} custom_data;

void handle_sigint(int sig) {
	(void)sig;
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

	dissect_packet(data->cmd, pkt, data->libs, data->custom_dissectors);
}

void execute_analize(command *cmd, raw_array *packets, shared_libs *libs, custom_dissectors *custom_diss) {
	/* ==============================  Getting args values from cmd  ================================ */
	custom_data custom_args = { .cmd = cmd, .packets = packets, .libs = libs, .custom_dissectors = custom_diss, .pcap_dump = NULL };
	char errbuff[PCAP_ERRBUF_SIZE];
	struct pcap_if *alldevs;
	struct bpf_program fp;
	bpf_u_int32 mask;
	bpf_u_int32 net;

	struct timespec start, end;
	double elapsed;
	
	int pkt_num = -1;
	long raw_tmp = str_to_num(get_raw_val(cmd, NUMBER_ARG));
	int tmp;
    char *filter_exp = get_raw_val(cmd, FILTER_ARG);
	char *dev = get_raw_val(cmd, DEVICE_ARG);
	char *read_file = get_raw_val(cmd, READ_FILE_ARG);
	char *write_file = get_raw_val(cmd, WRITE_FILE_ARG);
    int prom_mode = NULL == get_arg(cmd, NO_PROM_ARG);

	if (raw_tmp > INT_MAX || raw_tmp < INT_MIN) raise_error(LONG_TO_INT_CAST_ERROR, 0, NULL);
	else tmp = (int)raw_tmp;

    /* if -n not provided (or -n value not set) returns 0. Analizing 0 packets doesn't make sense, so assume to scan to infinity */
    if (0 != tmp) pkt_num = tmp;
	/* ============================================================================================== */

    clock_gettime(CLOCK_MONOTONIC, &start);
	if (NULL != read_file) {
        handle = pcap_open_offline(read_file, errbuff);
        if (NULL == handle) {
			raise_error(PCAP_FILE_ERROR, 0, ABSOLUTE_PATH_HINT, read_file);
			return;
		}
		printf(CAPTURE_FROM_FILE_MSG, read_file);
    } else {
		if (pcap_findalldevs(&alldevs, errbuff) == -1 && NULL != dev) {
			raise_error(PCAP_FINDALLDEVS_ERROR, 0, NULL, errbuff);
			return;
		}
		if (alldevs == NULL && NULL != dev) {
			raise_error(DEVICES_SCAN_ERROR, 0, NULL);
			return;
		}

		if (NULL == dev) dev = alldevs->name;
        if (NULL == dev) {
            raise_error(NO_DEVICE_FOUND, 0, NO_DEVICE_HINT, dev);
            mask = 0;
            net = 0;
			return;
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
        if (NULL == handle) {
			raise_error(NO_ACCESS_DEVICE_ERROR, 0, NULL, dev);
			return;
		}
        printf(CAPTURE_DEVICE_MSG, dev);
		pcap_freealldevs(alldevs);
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
	else {
		clock_gettime(CLOCK_MONOTONIC, &end);
		printf("\ntotal packets: %ld\n", packets->len);
		elapsed = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    	printf("elapsed time: %.6f seconds\n", elapsed);
	}

	if (NULL != custom_args.pcap_dump) pcap_dump_close(custom_args.pcap_dump);
	pcap_close(handle);
}
