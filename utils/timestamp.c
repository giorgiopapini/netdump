#include <stdio.h>
#include <time.h>
#include "timestamp.h"

void print_timestamp(struct timeval timestamp) {
	time_t rawtime = timestamp.tv_sec;
    struct tm *timeinfo = localtime(&rawtime);

    printf(
		"[%02d:%02d:%02d.%06ld] ",
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec,
        timestamp.tv_usec
	);
}