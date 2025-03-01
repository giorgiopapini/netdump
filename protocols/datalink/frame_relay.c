#include <stdio.h>
#include <arpa/inet.h>

#include "frame_relay.h"
#include "../nlpid_protos.h"
#include "../ethertypes.h"
#include "../../utils/visualizer.h"


int snap_hdr_len(const uint8_t *pkt) {
    uint8_t ea1 = pkt[0] & 0x01;
    uint8_t ea2 = pkt[1] & 0x01;
    uint8_t ea3, ea4;
    int hdr_len = 2;  /* default len */

    if (0 == ea2) {
        uint8_t ea3 = pkt[2] & 0x01;
        hdr_len = 3;

        if (0 == ea3) hdr_len = 4;
    }

    
    return hdr_len;
}

uint16_t extract_dlci(const uint8_t *pkt, int hdr_len) {
    uint16_t dlci = ((pkt[0] & 0xfc) << 2) | ((pkt[1] & 0xf0) >> 4);

    if (3 == hdr_len) dlci = (dlci << 7) | ((pkt[2] & 0xfe) >> 1);
    else if (4 == hdr_len) dlci = (dlci << 7) | ((pkt[3] & 0xfe) >> 1);
    return dlci;
}

void print_frelay_hdr(const uint8_t *pkt, uint32_t len) {
    int hdr_len = snap_hdr_len(pkt);
    int offset = 0x03 == pkt[hdr_len] ? hdr_len + 1 : hdr_len;
    uint16_t protocol = ntohs(FRELAY_PROTO(pkt, offset));

    printf(
        "dlci: %u, fecn: %u, becn: %u, de: %u",
        extract_dlci(pkt, hdr_len),
        FRELAY_FECN(pkt),
        FRELAY_BECN(pkt),
        FRELAY_DE(pkt)
    );

    if (protocol <= NLPID_THRESHOLD) printf(", nlpid: 0x%02x", protocol);
    else printf(", ethertype: 0x%04x", protocol);
}

void visualize_frelay_hdr(const uint8_t *pkt, uint32_t len) {
    char dlci[7];  /* 2 bytes, max = 65535'\0' 7 chars */
    char fecn[2];
    char becn[2];
    char de[2];
    char protocol_ether[7];  /* 0x0000'\0' 7 chars */
    char protocol_nlpid[5];  /* 0x00'\0' 4 chars */
    int hdr_len = snap_hdr_len(pkt);
    int offset = 0x03 == pkt[hdr_len] ? hdr_len + 1 : hdr_len;
    uint16_t protocol = ntohs(FRELAY_PROTO(pkt, offset));

    snprintf(dlci, sizeof(dlci), "%u", extract_dlci(pkt, hdr_len));
    snprintf(fecn, sizeof(fecn), "%u", FRELAY_FECN(pkt) ? 1 : 0);
    snprintf(becn, sizeof(becn), "%u", FRELAY_BECN(pkt) ? 1 : 0);
    snprintf(de, sizeof(de), "%u", FRELAY_DE(pkt) ? 1 : 0);

    if (protocol <= NLPID_THRESHOLD) snprintf(protocol_nlpid, sizeof(protocol_nlpid), "0x%02x", protocol);
    else snprintf(protocol_ether, sizeof(protocol_ether), "0x%04x", protocol);

    start_printing();
    print_field(FRELAY_DLCI_LABEL, dlci, 0);
    print_field(FRELAY_FECN_LABEL, fecn, 0);
    print_field(FRELAY_BECN_LABEL, becn, 0);
    print_field(FRELAY_DE_LABEL, de, 0);
    
    if (protocol <= NLPID_THRESHOLD) print_field(FRELAY_NLPID_LABEL, protocol_nlpid, 0);
    else print_field(FRELAY_ETHERTYPE_LABEL, protocol_ether, 0);
    end_printing();
}

protocol_info dissect_frelay(const uint8_t *pkt, uint32_t pkt_len, output_format fmt) {
    int hdr_len = snap_hdr_len(pkt);
    int offset = 0x03 == pkt[hdr_len] ? hdr_len + 1 : hdr_len;
    protocol_handler *table;
    uint16_t protocol = ntohs(FRELAY_PROTO(pkt, offset));
    
    SHOW_OUTPUT(pkt, pkt_len, fmt, print_frelay_hdr, visualize_frelay_hdr);

    if (protocol <= NLPID_THRESHOLD) table = nlpid_protos;
    else table = ethertypes;
    
    /* hdr_len + 2 (because 2 bytes long the protocol field, even if nlpid (i'm considering also the 0x00 byte)) */
    return (protocol_info){ .protocol = protocol, .offset = offset + 2, .table = table };
}