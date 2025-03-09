#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "shared_lib.h"
#include "../status_handler.h"


int is_active(shared_libs *libs, char *filename) {
    int i;

    if (NULL == libs->filenames) return 0;

    for (i = 0; i < libs->count; i ++) {
        if (NULL == libs->filenames[i]) continue;
        if (0 == strcmp(filename, libs->filenames[i])) return libs->statuses[i];
    }
    return 0;
}

void add_handle_to_libs(shared_libs *libs, void *new_handle) {
    libs->handles = (void **)realloc(libs->handles, (libs->count + 1) * sizeof(void *));
    if (NULL == libs->handles) raise_error(NULL_POINTER, 1, NULL, "libs->handles", __FILE__);

    libs->handles[libs->count] = new_handle;
}

shared_libs *load_shared_libs(const char *directory) {
    shared_libs *libs = (shared_libs *)malloc(sizeof(shared_libs));
    if (NULL == libs) raise_error(NULL_POINTER, 1, NULL, "libs", __FILE__);
    
    libs->handles = (void **)malloc(sizeof(void *));
    if (NULL == libs->handles) raise_error(NULL_POINTER, 1, NULL, "libs->handle", __FILE__);

    libs->filenames = (char **)malloc(sizeof(char *));
    if (NULL == libs->filenames) raise_error(NULL_POINTER, 1, NULL, "libs->filenames", __FILE__);
    
    libs->statuses = (int *)malloc(sizeof(int));
    if (NULL == libs->statuses) raise_error(NULL_POINTER, 1, NULL, "libs->statuses", __FILE__);

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

            libs->filenames = realloc(libs->filenames, (libs->count + 1) * sizeof(char *));
            if (!libs->filenames) {
                closedir(dir);
                raise_error(NULL_POINTER, 1, NULL, "libs->filenames", __FILE__);
                continue;
            }

            libs->filenames[libs->count] = strdup(entry->d_name);
            add_handle_to_libs(libs, handle);
            libs->statuses[libs->count] = 1;  /* lib is considered activated by default */

            libs->count ++;
        }
    }
    closedir(dir);

    return libs;
}

void destroy_shared_libs(shared_libs *libs) {
    int i;
    
    if (NULL != libs->filenames) {
        for (i = 0; i < libs->count; i ++) {
            if (NULL != libs->handles[i]) dlclose(libs->handles[i]);
            if (NULL != libs->filenames[i]) free(libs->filenames[i]);
        }
        free(libs->filenames);
    }

    if (NULL != libs->handles) free(libs->handles);    
    if (NULL != libs->statuses) free(libs->statuses);
    free(libs);
}