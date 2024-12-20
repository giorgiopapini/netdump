#include <stdio.h>
#include <pcap.h>

#include "save.h"
#include "../utils/packet.h"
#include "../status_handler.h"
#include "../utils/string_utils.h"

void execute_save(command *cmd, raw_array *packets) {
    pcap_t *handle;
    pcap_dumper_t *dumper;
    packet *tmp_pkt;
    char *dest_file = get_raw_val(cmd, DEST_FILE_ARG);
    int pkt_num = str_to_num(get_raw_val(cmd, NUMBER_ARG));
    int i;

    /* check if char *dest_file exists, otherwise create it, than execute pcap_dump_open() */

    if (packets->len <= 0) {
        raise_error(SAVING_EMPTY_PACKETS_ERROR, 0, NULL);
        return;
    }

    if (pkt_num > packets->len) {
        raise_error(INDEX_OUT_OF_BOUNDS, 0, NULL, pkt_num, packets->len);
        return;
    }

    if (pkt_num < 0) {  /* maybe pkt_num = -1 could signify to save ALL the data and not a particular packet */
        raise_error(NEGATIVE_N_PACKETS, 0, NULL, pkt_num);
        return;
    }

    handle = pcap_open_dead(DLT_EN10MB, 65535);  /* 65535 snaplen for DLT_EN10MB, but should be sufficient for every DLT (Datalink Type) */ 
    if (NULL == handle) raise_error(PCAP_HANDLE_ERROR, 1, NULL);

    dumper = pcap_dump_open(handle, dest_file);
    if (NULL == dumper) {
        pcap_close(handle);
        raise_error(PCAP_DUMP_FILE_ERROR, 1, NULL);
    }

    /* pcap_dump overwrites the previously stored data, maybe find a way to append it instead of overwriting it? */
    for (i = 0; i < packets->len; i ++/* check if char *dest_file exists, otherwise create it, than execute pcap_dump_open() */) {
        tmp_pkt = (packet *)packets->values[i];
        pcap_dump((uint8_t *) dumper, tmp_pkt->header, tmp_pkt->bytes);
    }

    pcap_dump_close(dumper);
    pcap_close(handle);

    print_success_msg(PACKETS_DUMP_SUCCESS);
}