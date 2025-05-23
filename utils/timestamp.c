#include "timestamp.h"

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "colors.h"
#include "../status_handler.h"


void print_timestamp(struct timeval timestamp) {
	time_t rawtime = timestamp.tv_sec;
    struct tm *timeinfo = localtime(&rawtime);

    CHECK_NULL_EXIT(timeinfo);
    printf(
		GREEN "[%02d:%02d:%02d.%06ld] " RESET_COLOR,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec,
        timestamp.tv_usec
	);
}