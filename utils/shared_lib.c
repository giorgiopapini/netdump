#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "shared_lib.h"
#include "../status_handler.h"
#include "string_utils.h"


shared_libs *create_shared_libs_obj() {
    shared_libs *libs = (shared_libs *)malloc(sizeof(shared_libs));
    if (NULL == libs) raise_error(NULL_POINTER, 1, NULL, "libs", __FILE__);

    libs->handles = NULL;
    libs->filenames = NULL;
    libs->statuses = NULL;
    libs->count = 0;
    return libs;
}

int is_active(shared_libs *libs, char *filename) {
    size_t i;

    if (NULL == libs->filenames) return 0;

    for (i = 0; i < libs->count; i ++) {
        if (NULL == libs->filenames[i]) continue;
        if (0 == strcmp(filename, libs->filenames[i])) return libs->statuses[i];
    }
    return 0;
}

void add_shared_lib(shared_libs *libs, void *new_handle, char *new_filename, int new_status) {
    libs->handles = (void **)realloc(libs->handles, (libs->count + 1) * sizeof(void *));
    if (NULL == libs->handles) raise_error(NULL_POINTER, 1, NULL, "libs->handles", __FILE__);
    
    libs->filenames = (char **)realloc(libs->filenames, (libs->count + 1) * sizeof(char *));
    if (NULL == libs->filenames) raise_error(NULL_POINTER, 1, NULL, "libs->filenames", __FILE__);

    libs->statuses = (int *)realloc(libs->statuses, (libs->count + 1) * sizeof(int));
    if (NULL == libs->statuses) raise_error(NULL_POINTER, 1, NULL, "libs->statuses", __FILE__);

    libs->handles[libs->count] = new_handle;
    libs->filenames[libs->count] = new_filename;
    libs->statuses[libs->count] = new_status;
    libs->count ++;
}

shared_libs *load_shared_libs(const char *directory) {
    shared_libs *libs = create_shared_libs_obj();
    if (NULL == libs) raise_error(NULL_POINTER, 1, NULL, "libs", __FILE__);

    struct dirent *entry;
    void *handle;
    char expanded_dir[4096];
    char path[8192];

    expand_tilde(directory, expanded_dir, sizeof(expanded_dir));

    DIR *dir = opendir(expanded_dir);
    if (NULL == dir) {
        raise_error(FOLDER_OPEN_ERROR, 0, NULL, expanded_dir);
        return libs;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".so") != NULL) {
            snprintf(path, sizeof(path), "%s/%s", expanded_dir, entry->d_name);

            handle = dlopen(path, RTLD_LAZY);
            if (!handle) {
                raise_error(LOADING_SHARED_LIB_ERROR, 0, NULL, entry->d_name, dlerror());
                continue;
            }

            add_shared_lib(libs, handle, strdup(entry->d_name), 1);
        }
    }
    closedir(dir);
    return libs;
}

void destroy_shared_libs(shared_libs *libs) {
    size_t i;
    
    if (NULL != libs->filenames) {
        for (i = 0; i < libs->count; i ++) {
            if (NULL != libs->handles[i]) dlclose(libs->handles[i]);
            if (NULL != libs->filenames[i]) free(libs->filenames[i]);
        }
        if (NULL != libs->filenames) free(libs->filenames);
    }

    if (NULL != libs->handles) free(libs->handles);    
    if (NULL != libs->statuses) free(libs->statuses);
    free(libs);
}