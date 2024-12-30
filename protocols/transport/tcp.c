#include <stdio.h>
#include <arpa/inet.h>

#include "tcp.h"
#include "../../utils/visualizer.h"
#include "../../utils/string_utils.h"


size_t tcp_hdr_len(const uint8_t *pkt) { return DATA_OFFSET(pkt) * 4; }

void print_tcp_hdr(const uint8_t *pkt) {
    printf("\nDUMMY TCP\n");
}

void visualize_tcp_hdr(const uint8_t *pkt) {
    char src_port[6];  /* 16 bit ==> max = 65536 (5 chars + '\0') */
    char dest_port[6];
    char seq[11];  /* 32 bit can represent a max of 4294967295'\0' ==> len = 11 */
    char ack_num[11];
    char data_offset[4];  /* 255'\0' ==> len = 4 */
    char reserved[5];  /* 0000'\0' (4 bits) ==> len = 5 */
    char cwr[2];  /* 1 bit */
    char ece[2];
    char urg[2];
    char ack[2];
    char psh[2];
    char rst[2];
    char syn[2];
    char fin[2];
    char window_size[6];
    char checksum[7];  /* 0x0000'\0' */
    char urgent_pointer[7];
    /* char options[];  */

    snprintf(src_port, sizeof(src_port), "%u", ntohs(SRC_PORT(pkt)));
    snprintf(dest_port, sizeof(dest_port), "%u", ntohs(DEST_PORT(pkt)));
    snprintf(seq, sizeof(seq), "%u", ntohl(SEQUENCE(pkt)));
    snprintf(ack_num, sizeof(ack_num), "%u", ntohl(ACK_NUM(pkt)));
    snprintf(data_offset, sizeof(data_offset), "%u", DATA_OFFSET(pkt));
    uint_to_bin_str(reserved, RESERVED(pkt), sizeof(reserved));
    snprintf(cwr, sizeof(cwr), "%d", (FLAGS(pkt) & CWR) ? 1 : 0);
    snprintf(ece, sizeof(ece), "%d", (FLAGS(pkt) & ECE) ? 1 : 0);
    snprintf(urg, sizeof(urg), "%d", (FLAGS(pkt) & URG) ? 1 : 0);
    snprintf(ack, sizeof(ack), "%d", (FLAGS(pkt) & ACK) ? 1 : 0);
    snprintf(psh, sizeof(psh), "%d", (FLAGS(pkt) & PSH) ? 1 : 0);
    snprintf(rst, sizeof(rst), "%d", (FLAGS(pkt) & RST) ? 1 : 0);
    snprintf(syn, sizeof(syn), "%d", (FLAGS(pkt) & SYN) ? 1 : 0);
    snprintf(fin, sizeof(fin), "%d", (FLAGS(pkt) & FIN) ? 1 : 0);
    snprintf(window_size, sizeof(window_size), "%u", ntohs(WINDOW_SIZE(pkt)));
    snprintf(checksum, sizeof(checksum), "0x%04x", ntohs(CHECKSUM(pkt)));
    snprintf(urgent_pointer, sizeof(urgent_pointer), "0x%04x", ntohs(URGENT_POINTER(pkt)));

    start_printing();
    print_hdr_info(TCP_HEADER_LABEL, NULL);
    print_field(SRC_PORT_LABEL, src_port, 0);
    print_field(DEST_PORT_LABEL, dest_port, 0);
    print_field(SEQUENCE_LABEL, seq, 0);
    print_field(ACK_NUM_LABEL, ack_num, 0);
    print_field(DATA_OFFSET_LABEL, data_offset, 0);
    print_field(RESERVED_LABEL, reserved, 0);
    print_field(CWR_LABEL, cwr, 0);
    print_field(ECE_LABEL, ece, 0);
    print_field(URG_LABEL, urg, 0);
    print_field(ACK_LABEL, ack, 0);
    print_field(PSH_LABEL, psh, 0);
    print_field(RST_LABEL, rst, 0);
    print_field(SYN_LABEL, syn, 0);
    print_field(FIN_LABEL, fin, 0);
    print_field(WINDOW_SIZE_LABEL, window_size, 0);
    print_field(CHECKSUM_LABEL, checksum, 0);
    print_field(URGENT_POINTER_LABEL, urgent_pointer, 0);
    /* print_field(OPTIONS_LABEL, options, 0); */
    end_printing();
}