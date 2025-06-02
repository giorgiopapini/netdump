#ifndef DLT_PROTOS_H
#define DLT_PROTOS_H

#include "../utils/hashmap.h"

/* around 300 known DLTs */
#define DLT_PROTOS_BUCKETS_NUM  512


extern hashmap *dlt_protos;

void load_dlt_protos(void);

#endif