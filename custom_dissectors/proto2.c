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

void print_proto(const uint8_t *pkt, uint32_t len) {
    printf("EXECUTED PROTO2\n");
}

void visualize_proto(const uint8_t *pkt, uint32_t len) {
    start_printing();
    print_field("AAAA", "prova1 diocan", 0);
    print_field("AAAA", "prova2 diocan", 0);
    print_field("AAAA", "prova3 diocan", 0);
    print_field("AAAA", "prova4 diocan", 0);
    end_printing();
}

protocol_info dissect_proto(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_proto, visualize_proto);
    return NO_ENCAP_PROTO;
}

protocol_handler_mapping **get_custom_protocols_mapping() {
    protocol_handler_mapping **arr = create_mappings_arr();
    
    add_mapping(
        &arr,
        create_protocol_handler_mapping(
            create_protocol_handler(6, PROTOCOL_LAYER_TRANSPORT, dissect_proto, "PROTO2"),
            IP_PROTOS
        )
    );

    return arr;
}