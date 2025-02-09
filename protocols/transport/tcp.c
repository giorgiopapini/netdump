#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "tcp.h"
#include "../net_ports.h"
#include "../../utils/visualizer.h"
#include "../../utils/string_utils.h"


size_t tcp_options_len(const uint8_t *pkt) { return (DATA_OFFSET(pkt) * 4) - 20; }  /* 20 = standard tcp header length */

void print_tcp_options(const uint8_t *pkt) {
    size_t opts_len = tcp_options_len(pkt);
    uint8_t *options = OPTIONS(pkt);
    uint8_t kind;
    uint8_t length;
    int i = 0;
    int j = 0;

    printf(", options: [");

    while (i < opts_len) {
        kind = options[i];
        if (0 == kind) {
            printf("eol");
            break;
        }
        else if (1 == kind) {
            printf("nop");
            i ++;
        }
        else {
            if (i + 1 >= opts_len) {
                printf("(malformed_option: kind %d)", kind);
                break;
            }
            length = options[i + 1];
            if (length < 2 || i + length > opts_len) {
                printf("(malformed_option: kind %d, length %d)", kind, length);
                break;
            }

            switch (kind) {
            case 2: {
                if (length == 4) {
                    uint16_t mss = ntohs(*(uint16_t *)(options + i + 2));
                    printf("mss: %d", mss);
                }
                break;
            }
            case 3: {
                if (length == 3) {
                    uint8_t scale = options[i + 2];
                    printf("ws: %d", scale);
                }
                break;
            }
            case 4: {
                printf("sackOK");
                break;
            }
            case 5: {
                printf("sack");
                for (j = 2; j < length; j += 8) {
                    if (j + 7 < length) {
                        uint32_t left_edge = ntohl(*(uint32_t *)(options + i + j));
                        uint32_t right_edge = ntohl(*(uint32_t *)(options + i + j + 4));
                        printf(" (left_edge: %u, right_edge: %u)", left_edge, right_edge);
                    }
                }
                break;
            }
            case 8: {
                if (length == 10) {
                    uint32_t ts_val = ntohl(*(uint32_t *)(options + i + 2));
                    uint32_t ts_echo = ntohl(*(uint32_t *)(options + i + 6));
                    printf("TSval: %u, TSecr: %u", ts_val, ts_echo);
                }
                break;
            }
            case 34: printf("tcp_fast_open"); break;
            default:
                printf("data:");
                for (j = 2; j < length; j++) {
                    printf(" %02x", options[i + j]);
                }
                break;
            }
            i += length;
        }
        if ((i + 1) < opts_len) printf(", ");
    }
    printf("]");
}

void print_tcp_hdr(const uint8_t *pkt) {
    char flags[41] = "";  /* IMPORTANT! Initialize flags to empty str, otherwiese strcat could lead to undefined behaviours */
    printf("src port: %u, dest port: %u", ntohs(SRC_PORT(pkt)), ntohs(DEST_PORT(pkt)));
    
    printf(", flags: [");
    if (FLAGS(pkt) & CWR) strcat(flags, "CWR, ");
    if (FLAGS(pkt) & ECE) strcat(flags, "ECE, ");
    if (FLAGS(pkt) & URG) strcat(flags, "URG, ");
    if (FLAGS(pkt) & ACK) strcat(flags, "ACK, ");
    if (FLAGS(pkt) & PSH) strcat(flags, "PSH, ");
    if (FLAGS(pkt) & RST) strcat(flags, "RST, ");
    if (FLAGS(pkt) & SYN) strcat(flags, "SYN, ");
    if (FLAGS(pkt) & FIN) strcat(flags, "FIN, ");
    flags[strlen(flags) - 2] = '\0';    /* remove last ", " chars */
    printf("%s]", flags);

    printf(", seq: %u", ntohl(SEQUENCE(pkt)));
    printf(", ack: %u", ntohl(ACK_NUM(pkt)));
    printf(", win: %u", ntohs(WINDOW_SIZE(pkt)));
    printf(", cksum: 0x%04x", ntohs(CHECKSUM(pkt)));
    if (FLAGS(pkt) & URG) printf(", urgent_pointer: 0x%04x", ntohs(URGENT_POINTER(pkt)));
    if (0 < tcp_options_len(pkt)) print_tcp_options(pkt);
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
    print_hdr_info(TCP_HEADER_LABEL, "Options fields not represented in ascii art");
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

protocol_info dissect_tcp(const uint8_t *pkt, uint32_t pkt_len, const char *proto_name, output_format fmt) {
    protocol_info proto_info;
    SHOW_OUTPUT(pkt, fmt, proto_name, print_tcp_hdr, visualize_tcp_hdr);

    /* Pure TCP handshake (SYN, SYN-ACK, ACK) packets contain no HTTP data. */
    proto_info.offset = (DATA_OFFSET(pkt) * 4);
    proto_info.table = net_ports;

    if (((FLAGS(pkt) & (ACK | PSH)) == (ACK | PSH)) || ((FLAGS(pkt) & ACK) == ACK)) {
        if (IS_WELL_DEFINED_PORT(ntohs(DEST_PORT(pkt)))) proto_info.protocol = ntohs(DEST_PORT(pkt));
        else if (IS_WELL_DEFINED_PORT(ntohs(SRC_PORT(pkt)))) proto_info.protocol = ntohs(SRC_PORT(pkt));
        else proto_info = NO_ENCAP_PROTO;
    }
    else proto_info = NO_ENCAP_PROTO;

    return proto_info;
}