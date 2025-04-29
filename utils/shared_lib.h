#ifndef SHARED_LIB_H
#define SHARED_LIB_H

#include <dirent.h>

typedef struct shared_libs {
    void **handles;
    char **filenames;
    int *statuses;  /* 1 = lib active, 0 = lib deactive (but still in memory) */
    size_t count;
} shared_libs;


shared_libs *create_shared_libs_obj();
int is_active(shared_libs *libs, char *filename);
void add_shared_lib(shared_libs *libs, void *new_handle, char *new_filename, int new_status);
shared_libs *load_shared_libs(const char *folder);
void destroy_shared_libs(shared_libs *libs);

#endif