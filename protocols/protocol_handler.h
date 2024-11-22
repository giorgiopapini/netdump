#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include <stdint.h>
#include <stddef.h>

/* { PROTOCOL, sizeof(proto_hdr), print_proto_header } */

typedef struct field {
	const uint16_t start;
	const size_t len;
	int ntoh;  /* if ntoh flag is set, than the field should be read using noths or ntohl depending on how many bytes the field has */
} field;

typedef struct protocol_info {
	int protocol;
	size_t hdr_size;
	field encap_type_range;
    void (*print_header)(const uint8_t *pkt);   /* Once the callback function is stored there is no need to "know" the packet_hdr directly. */
} protocol_info;


int get_field(const uint8_t *pkt, field byte_segment);

#endif