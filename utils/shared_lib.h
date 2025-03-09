#ifndef SHARED_LIB_H
#define SHARED_LIB_H

#include <dirent.h>


typedef struct shared_libs {
    void **handles;
    char **filenames;
    int *statuses;  /* 1 = lib active, 0 = lib deactive (but still in memory) */
    int count;
} shared_libs;


int is_active(shared_libs *libs, char *filename);
void add_handle_to_libs(shared_libs *libs, void *handle);
shared_libs *load_shared_libs(const char *folder);
void destroy_shared_libs(shared_libs *libs);

#endif