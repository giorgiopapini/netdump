#include <pcap.h>

#include "datalink_handler.h"
#include "datalink/ether.h"  /* find a way to import all the protocols using one single include directive */


protocol_info datalink_protocols[] = {
    { DLT_EN10MB, sizeof(ether_hdr), { .start = 12, .len = 2, .ntoh = 1 }, print_ether_hdr },
};


protocol_info dissect_datalink(int datalink_type) {
    int i;
    for (i = 0; i < sizeof(datalink_protocols) / sizeof(datalink_protocols[0]); i ++) {
        if (datalink_protocols[i].protocol == datalink_type) return datalink_protocols[i]; 
    }
    return (protocol_info){ 0, 0, 0, 0, 0, NULL };
}