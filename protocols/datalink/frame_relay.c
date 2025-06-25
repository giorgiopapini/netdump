#include "frame_relay.h"

#include <stdio.h>

#include "../../libs/libnetdump/visualizer.h"
#include "../../libs/libnetdump/protocol.h"


static size_t _snap_hdr_len(const uint8_t *pkt);
static uint16_t _extract_dlci(const uint8_t *pkt, size_t hdr_len);
static void _print_frelay_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_frelay_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static size_t _snap_hdr_len(const uint8_t *pkt) {
    size_t hdr_len;
    uint8_t ea2;
    uint8_t ea3;

    hdr_len = 2;
    ea2 = pkt[1] & 0x01;

    if (ea2 == 0) {
        ea3 = pkt[2] & 0x01;
        hdr_len = 3;
        if (ea3 == 0) hdr_len = 4;
    }
    return hdr_len;
}

static uint16_t _extract_dlci(const uint8_t *pkt, size_t hdr_len) {
    uint16_t dlci;
    dlci = ((uint16_t)((pkt[0] & 0xfc) << 2) | (uint16_t)((pkt[1] & 0xf0) >> 4));

    if (3 == hdr_len) dlci = ((uint16_t)(dlci << 7) | (uint16_t)((pkt[2] & 0xfe) >> 1));
    else if (4 == hdr_len) dlci = ((uint16_t)(dlci << 7) | (uint16_t)((pkt[3] & 0xfe) >> 1));
    return dlci;
}

static void _print_frelay_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    size_t offset;
    uint16_t protocol;

    if (!pkt) return;
    if (hdr_len > pkt_len) return;

    offset = 0x03 == pkt[hdr_len] ? hdr_len + 1 : hdr_len;
    if ((offset + (size_t)1) > pkt_len) return;  /* +1 is needed because FRELAY_PROTO(...) access offset + 1 */

    protocol = FRELAY_PROTO(pkt, offset);

    printf(
        "dlci: %u, fecn: %u, becn: %u, de: %u",
        _extract_dlci(pkt, hdr_len),
        FRELAY_FECN(pkt),
        FRELAY_BECN(pkt),
        FRELAY_DE(pkt)
    );

    /* safe to cast protocol to (uint8_t) because of the bound checking condition */
    if (protocol <= NLPID_THRESHOLD) printf(", nlpid: 0x%02x", (uint8_t)protocol);
    else printf(", ethertype: 0x%04x", protocol);
}

static void _visualize_frelay_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {    
    char dlci[7];  /* 2 bytes, max = 65535'\0' 7 chars */
    char fecn[2];
    char becn[2];
    char de[2];
    char protocol_ether[7];  /* 0x0000'\0' 7 chars */
    char protocol_nlpid[5];  /* 0x00'\0' 4 chars */
    size_t offset;
    uint16_t protocol;

    if (!pkt) return;
    if (hdr_len > pkt_len) return;

    offset = 0x03 == pkt[hdr_len] ? hdr_len + 1 : hdr_len;
    if ((offset + (size_t)1) > pkt_len) return;  /* +1 is needed because FRELAY_PROTO(...) access offset + 1 */

    protocol = FRELAY_PROTO(pkt, offset);

    snprintf(dlci, sizeof(dlci), "%u", _extract_dlci(pkt, hdr_len));
    snprintf(fecn, sizeof(fecn), "%u", FRELAY_FECN(pkt) ? 1 : 0);
    snprintf(becn, sizeof(becn), "%u", FRELAY_BECN(pkt) ? 1 : 0);
    snprintf(de, sizeof(de), "%u", FRELAY_DE(pkt) ? 1 : 0);

    /* safe to cast protocol to (uint8_t) because of the bound checking condition */
    if (protocol <= NLPID_THRESHOLD) snprintf(protocol_nlpid, sizeof(protocol_nlpid), "0x%02x", (uint8_t)protocol);
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

protocol_info dissect_frelay(const uint8_t *pkt, size_t pkt_len, output_format fmt) {
    size_t hdr_len;
    size_t offset;
    int table_num;
    uint16_t protocol;

    if (!pkt) return NO_ENCAP_PROTO;

    hdr_len = _snap_hdr_len(pkt);
    if (hdr_len > pkt_len) return NO_ENCAP_PROTO;

    offset = 0x03 == pkt[hdr_len] ? hdr_len + 1 : hdr_len;
    if ((offset + (size_t)1) > pkt_len) return NO_ENCAP_PROTO;  /* +1 is needed because FRELAY_PROTO(...) access offset + 1 */

    protocol = FRELAY_PROTO(pkt, offset);
    
    SHOW_OUTPUT(pkt, pkt_len, hdr_len, fmt, _print_frelay_hdr, _visualize_frelay_hdr);

    if (protocol <= NLPID_THRESHOLD) table_num = NLPID_PROTOS;
    else table_num = ETHERTYPES;
    
    /* hdr_len + 2 (because 2 bytes long the protocol field, even if nlpid (i'm considering also the 0x00 byte)) */
    return (protocol_info){ .protocol = protocol, .offset = offset + 2, .proto_table_num = table_num };
}