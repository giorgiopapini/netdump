#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <stddef.h>
#include "protocols/ether.h"

void visualize(size_t size, void *packet);
void visualize_ether(size_t size, void *packet);


#endif