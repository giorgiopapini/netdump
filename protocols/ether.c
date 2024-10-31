#include <stdio.h>
#include <arpa/inet.h>

#include "ether.h"
#include "../ethertypes.h"
#include "../utils/raw_array.h"
#include "../utils/lookup_table.h"


lookup_table ethers = {
	{ .key = ETHERTYPE_8023, .value = "8023" },
};

void print_ether_hdr(const uint8_t *pkt) {
    ether_hdr *ether_header = (ether_hdr *)pkt;
    
	/* printing src (MAC) > dest (MAC) */
	for (int i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", ether_header->src_addr[i] & 0xff);
	}
    printf("%02x", ether_header->src_addr[5]);
    printf(" > ");
	for (int i = 0; i < 5; i ++) {      /* the last 8 bits are printed after without the trailing ':' */
		printf("%02x:", ether_header->dest_addr[i] & 0xff);
	}
    printf("%02x", ether_header->dest_addr[5] & 0xff);
	
	/* printing ethertype */
	printf(", ethertype: 0x%04x", ntohs(ether_header->ethertype));
	/* use 'get_value' function to get the entry of lookup table. Use LEN(type, ...) macro defined in 'raw_array.h' to get the
	length of the lookup table */

	printf("\n");
}