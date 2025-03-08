#ifndef SHARED_LIB_H
#define SHARED_LIB_H

#include <dirent.h>


typedef struct shared_libs {
    void **handles;
    int count;
} shared_libs;


void add_handle_to_libs(shared_libs *libs, void *handle);
shared_libs *load_shared_libs(const char *folder);
void destroy_shared_libs(shared_libs *libs);

#endif