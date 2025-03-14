#include <stdio.h>

/* start of modules inside of the supposed "netdump-devel" */
#include "../utils/protocol.h"
#include "../protocols/proto_tables_nums.h"

/* 
    Non posso usare direttamente il puntatore alle table, poichè essendo una shared library verrà compilata singolarmente, quindi
    gli address saranno diversi, la funzione dissector add deve eseguire una conversione
*/

#include "../utils/visualizer.h"
#include "../utils/string_utils.h"
/* end of modules inside of the supposed "netdump-devel" */

void print_tcp_prova(const uint8_t *pkt, uint32_t len) {
    printf("EXECUTED TCP PROVA");
}

void visualize_tcp_prova(const uint8_t *pkt, uint32_t len) {
    start_printing();
    print_field("SRC_PORT", "5000", 0);
    print_field("DEST_PORT", "80", 0);
    print_field("PROVA1", "prova1", 0);
    print_field("PROVA2", "prova2", 0);
    end_printing();
}

protocol_info dissect_tcp_prova(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_tcp_prova, visualize_tcp_prova);
    return NO_ENCAP_PROTO;
}

protocol_handler_mapping **get_custom_protocols_mapping() {
    protocol_handler_mapping **arr = create_mappings_arr();
    
    add_mapping(
        &arr,
        create_protocol_handler_mapping(
            create_protocol_handler(IPPROTO_TCP, PROTOCOL_LAYER_TRANSPORT, dissect_tcp_prova, "TCP_PROVA"),
            IP_PROTOS
        )
    );

    return arr;
}