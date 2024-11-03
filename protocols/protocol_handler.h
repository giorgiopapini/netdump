#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include <stdint.h>
#include <stddef.h>

/* { PROTOCOL, sizeof(proto_hdr), print_proto_header } */
typedef struct protocol_info {
	int protocol;
	size_t size;
    void (*print_header)(const uint8_t *pkt);   /* Once the callback function is stored there is no need to "know" the packet_hdr directly. */
} protocol_info;


#endif