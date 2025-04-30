#include <stdio.h>
#include <string.h>

#include "ether.h"
#include "../ethertypes.h"
#include "../../utils/visualizer.h"
#include "../../utils/formats.h"
#include "../proto_tables_nums.h"


void print_ether_hdr(const uint8_t *pkt, size_t pkt_len);
void visualize_ether_hdr(const uint8_t *pkt, size_t pkt_len);

void print_ether_hdr(const uint8_t *pkt, size_t pkt_len) {
    (void)pkt_len;
    /* ========================= printing src (MAC) > dest (MAC) ========================= */
    print_mac(SRC_ADDR(pkt));
    printf(" > ");
	print_mac(DEST_ADDR(pkt));
    /* =================================================================================== */

	/* =============================== printing ethertype ================================ */
	printf(", ethertype: 0x%04x", ETHERTYPE(pkt)); 
    /* =================================================================================== */
}

void visualize_ether_hdr(const uint8_t *pkt, size_t pkt_len) {
    char dest_addr[MAC_ADDR_STR_LEN];
    char src_addr[MAC_ADDR_STR_LEN];
    char ethertype[7];  /* 0xXXXX'\0' */
    (void)pkt_len;
    
    snprintf(dest_addr, sizeof(dest_addr), MAC_ADDR_FORMAT, MAC_TO_STR(DEST_ADDR(pkt)));
    snprintf(src_addr, sizeof(src_addr), MAC_ADDR_FORMAT, MAC_TO_STR(SRC_ADDR(pkt)));
    snprintf(ethertype, sizeof(ethertype), "0x%04x", ETHERTYPE(pkt));

    start_printing();
    print_field(DEST_ADDR_LABEL, dest_addr, 0);
    print_field(SRC_ADDR_LABEL, src_addr, 0);
    print_field(ETHERTYPE_LABEL, ethertype, 0);
    end_printing();
}

protocol_info dissect_ether(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_ether_hdr, visualize_ether_hdr);
    return (protocol_info){ .protocol = ETHERTYPE(pkt), .offset = ETHER_LEN, .proto_table_num = ETHERTYPES };
}