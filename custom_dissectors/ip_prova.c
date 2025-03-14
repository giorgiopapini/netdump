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

void print_ipv4_prova(const uint8_t *pkt, uint32_t len) {
    printf("EXECUTED IPV4 PROVA");
}

void visualize_ipv4_prova(const uint8_t *pkt, uint32_t len) {
    start_printing();
    print_field("SRC", "192.168.1.1", 0);
    print_field("DEST", "192.168.1.2", 0);
    print_field("PROVA1", "prova1", 0);
    print_field("PROVA2", "prova2", 0);
    end_printing();
}

protocol_info dissect_ipv4_prova(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_ipv4_prova, visualize_ipv4_prova);
    return (protocol_info){ 
        .protocol = (*((uint8_t *)(pkt + 9))), 
        .offset = ((*((uint8_t *)(pkt)) & 0x0f) * 4), 
        .proto_table_num = IP_PROTOS
    };
}

protocol_handler_mapping **get_custom_protocols_mapping() {
    protocol_handler_mapping **arr = create_mappings_arr();
    
    add_mapping(
        &arr,
        create_protocol_handler_mapping(
            create_protocol_handler(ETHERTYPE_IP, PROTOCOL_LAYER_NETWORK, dissect_ipv4_prova, "IPV4_PROVA"),
            ETHERTYPES
        )
    );

    return arr;
}