#include <stdio.h>
#include <pcap.h>

#include "devlist.h"
#include "../status_handler.h"
#include "../utils/colors.h"

void execute_devlist(command *cmd) {
    printf("\n");

    pcap_if_t *alldevs;
    pcap_if_t *device;
    char errbuff[PCAP_ERRBUF_SIZE];

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