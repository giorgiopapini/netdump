#include "devlist.h"

#include <stdio.h>
#include <pcap/pcap.h>

#include "../status_handler.h"
#include "../utils/colors.h"
#include "../utils/formats.h"

void execute_devlist(void) {
    pcap_if_t *alldevs;
    pcap_if_t *device;
    char errbuff[PCAP_ERRBUF_SIZE];

    printf("\n");
    if (-1 == pcap_findalldevs(&alldevs, errbuff)) {
        raise_error(DEVICES_SCAN_ERROR, 0, NULL);
        return;
    }

    for (device = alldevs; NULL != device; device = device->next) {
        printf(PREFIX_STR);
        printf(GREEN "%s" RESET_COLOR, device->name);

        if (device->description) printf(" (%s)", device->description);
        else printf(" (No description available)");

        printf("\n");
    }

    pcap_freealldevs(alldevs);
    printf("\n");
}