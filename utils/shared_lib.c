#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "shared_lib.h"
#include "../status_handler.h"


void add_handle_to_libs(shared_libs *libs, void *new_handle) {
    libs->handles = (void **)realloc(libs->handles, (libs->count + 1) * sizeof(void *));
    if (NULL == libs->handles) raise_error(NULL_POINTER, 1, NULL, "libs->handles", __FILE__);

    libs->handles[libs->count] = new_handle;
    libs->count ++;
}

shared_libs *load_shared_libs(const char *directory) {
    shared_libs *libs = (shared_libs *)malloc(sizeof(shared_libs));
    if (NULL == libs) raise_error(NULL_POINTER, 1, NULL, "libs", __FILE__);
    
    libs->handles = (void **)malloc(sizeof(void *));
    if (NULL == libs->handles) raise_error(NULL_POINTER, 1, NULL, "libs->handle", __FILE__);
    
    libs->count = 0;

    struct dirent *entry;
    void *handle;
    char path[512];
    
    DIR *dir = opendir(directory);
    if (dir == NULL) raise_error(CSTM_DISSECTORS_FOLDER_ERROR, 1, NULL);

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".so") != NULL) {
            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

            handle = dlopen(path, RTLD_LAZY);
            if (!handle) {
                raise_error(LOADING_SHARED_LIB_ERROR, 0, NULL, entry->d_name, dlerror());
                continue;
            }

            add_handle_to_libs(libs, handle);
        }
    }
    closedir(dir);

    return libs;
}

void destroy_shared_libs(shared_libs *libs) {
    int i;
    
    for (i = 0; i < libs->count; i ++) dlclose(libs->handles[i]);
    free(libs->handles);
    free(libs);
}