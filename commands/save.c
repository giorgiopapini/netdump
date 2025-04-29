#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pcap.h>

#include "save.h"
#include "../utils/packet.h"
#include "../status_handler.h"
#include "../utils/string_utils.h"


void print_cwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) printf("Current working directory: %s\n", cwd);
    else raise_error(CURR_WORK_DIR_ERROR, 1, NULL);
}

void execute_save(command *cmd, raw_array *packets) {
    pcap_t *handle;
    pcap_dumper_t *dumper;
    packet *tmp_pkt;
    char *dest_path = get_raw_val(cmd, DEST_FILE_ARG);
    long raw_pkt_num = str_to_num(get_raw_val(cmd, NUMBER_ARG));
    size_t pkt_num;  /* if pkt_num == 0 than save ALL the packets */
    size_t i;

    print_cwd(); 

    if (raw_pkt_num < 0) { raise_error(NEGATIVE_N_PACKETS, 0, NULL, raw_pkt_num); return; }
    else pkt_num = (size_t)raw_pkt_num;

    if (packets->len <= 0) { raise_error(SAVING_EMPTY_PACKETS_ERROR, 0, NULL); return; }
    if (pkt_num > packets->len) { raise_error(INDEX_OUT_OF_BOUNDS, 0, NULL, pkt_num, packets->len); return; }

    handle = pcap_open_dead(DLT_EN10MB, 65535);  /* 65535 snaplen for DLT_EN10MB, but should be sufficient for every DLT (Datalink Type) */ 
    if (NULL == handle) raise_error(PCAP_HANDLE_ERROR, 1, NULL);

    dumper = pcap_dump_open(handle, dest_path);
    if (NULL == dumper) {
        pcap_close(handle);
        raise_error(PCAP_DUMP_FILE_ERROR, 1, NULL);
    }

    /* pcap_dump overwrites the previously stored data, maybe find a way to append it instead of overwriting it? */
    if (0 != pkt_num) {
        tmp_pkt = get(packets, pkt_num);
        pcap_dump((uint8_t *) dumper, tmp_pkt->header, tmp_pkt->bytes);
    }
    else {
        for (i = 0; i < packets->len; i ++) {
            tmp_pkt = (packet *)packets->values[i];
            pcap_dump((uint8_t *) dumper, tmp_pkt->header, tmp_pkt->bytes);
        }
    }

    pcap_dump_close(dumper);
    pcap_close(handle);

    print_success_msg(PACKETS_DUMP_SUCCESS);
}