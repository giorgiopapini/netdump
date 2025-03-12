#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "dissectors.h"
#include "../status_handler.h"
#include "../utils/formats.h"
#include "../utils/colors.h"
#include "../utils/string_utils.h"


void change_dissector_status(int new_status, char *filenames, shared_libs *libs) {
    char *token;
    char *trimmed;
    int target_index = -1;
    int i;

    if (NULL == filenames) {
        for (i = 0; i < libs->count; i ++) libs->statuses[i] = new_status;
        if (0 == new_status) print_success_msg(DISSECTOR_DEACTIVATED_SUCCESS);
        else print_success_msg(DISSECTOR_ACTIVATED_SUCCESS);
        return;
    }

    token = strtok(filenames, STRINGS_SEPARATOR);
    while (token != NULL) {
        target_index = -1;
        trimmed = get_trimmed_str(token);

        if (NULL != libs->filenames) {
            for (i = 0; i < libs->count; i ++) {
                if (NULL != libs->filenames[i] && 0 == strcmp(trimmed, libs->filenames[i])) {
                    target_index = i;
                    break;
                }
            }
        }

        if (-1 == target_index) raise_error(LIB_NOT_FOUND_ERROR, 0, NULL, trimmed);
        else {
            if (0 == new_status) print_success_msg(DISSECTOR_DEACTIVATED_SUCCESS);
            else print_success_msg(DISSECTOR_ACTIVATED_SUCCESS);
            libs->statuses[target_index] = new_status;
        }

        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
}

void add_and_load_dissectors(shared_libs *libs, custom_dissectors *dissectors, char *paths) {
    char *token;
    char *trimmed;
    char *filename;
    int found = 0;
    struct dirent *entry;
    void *handle;
    char path[512];

    if (NULL == paths) return;
    
    DIR *dir = opendir(CUSTOM_DISSECTORS_PATH);
    

    token = strtok(paths, STRINGS_SEPARATOR);
    while (token != NULL) {
        found = 0;
        trimmed = get_trimmed_str(token);
        filename = get_filename(trimmed);

        while ((entry = readdir(dir)) != NULL) {
            if (0 == strcmp(entry->d_name, filename)) {
                found = 1;
                break;
            }
        }
        rewinddir(dir);

        if (!found) {
            snprintf(path, sizeof(path), "%s/%s", CUSTOM_DISSECTORS_PATH, filename);
            if (copy_file(trimmed, path)) {
                handle = dlopen(path, RTLD_LAZY);
                if (!handle) {
                    raise_error(LOADING_SHARED_LIB_ERROR, 0, NULL, filename, dlerror());
                    continue;
                }

                add_shared_lib(libs, handle, strdup(filename), 1);
                load_dissector(dissectors, handle, strdup(filename));
                print_success_msg(DISSECTOR_LOADED_SUCCESS);
            }
            else raise_error(FILE_COPY_ERROR, 0, NULL, trimmed, path);
        }
        else raise_error(FILE_OVERWRITE_ERROR, 0, FILE_OVERWRITE_HINT, filename, CUSTOM_DISSECTORS_PATH);

        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
};

void delete_dissector(shared_libs *libs, char *filenames) {
    char *token;
    char *trimmed;
    int target_index = -1;
    char path[512];
    int i;

    if (NULL == filenames) return;

    token = strtok(filenames, STRINGS_SEPARATOR);
    while (token != NULL) {
        target_index = -1;
        trimmed = get_trimmed_str(token);
        snprintf(path, sizeof(path), "%s/%s", CUSTOM_DISSECTORS_PATH, trimmed);

        if (NULL != libs->filenames) {
            for (i = 0; i < libs->count; i ++) {
                if (NULL != libs->filenames[i] && 0 == strcmp(trimmed, libs->filenames[i])) {
                    target_index = i;
                    break;
                }
            }
        }

        if (-1 == target_index) raise_error(LIB_NOT_FOUND_ERROR, 0, NULL, trimmed);
        else {
            if (0 != remove(path)) raise_error(DELETE_FILE_ERROR, 0, NULL, path);
            else {
                dlclose(libs->handles[target_index]);
                libs->handles[target_index] = NULL;
                print_success_msg(DISSECTOR_DELETED_SUCCESS);
                free(libs->filenames[target_index]);
                libs->filenames[target_index] = NULL;
                libs->statuses[target_index] = 0;
            }
        }

        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
}

void print_dissectors_list(shared_libs *libs) {
    int empty = 1;
    int i;

    for (i = 0; i < libs->count; i ++) {
        if (NULL != libs->filenames[i]) {
            empty = 0;
            break;
        }
    }

    if (empty) {
        print_warning_msg(DISSECTORS_EMPTY_WARNING);
        return;
    }

    printf("\n");
    for (i = 0; i < libs->count; i ++) {
        if (NULL != libs->filenames[i]) {
            printf(PREFIX_STR);
            printf(GREEN "%s" RESET_COLOR, libs->filenames[i]);
            
            if (libs->statuses[i]) printf(" (%s)", LIB_ACTIVE);
            else printf(" (%s)", LIB_DEACTIVE);
            printf("\n");
        }
    }
    printf("\n");
}

void execute_dissectors(command *cmd, shared_libs *libs, custom_dissectors *custom_dissectors) {
    int show_list = (NULL != get_arg(cmd, DISSECTOR_LIST_ARG) || 0 == cmd->n_hashes);
    arg *activate_arg = get_arg(cmd, ACTIVATE_LIB_ARG);
    arg *deactivate_arg = get_arg(cmd, DEACTIVATE_LIB_ARG);
    char *add_paths = get_raw_val(cmd, ADD_DISSECTOR_ARG);
    char *delete_filenames = get_raw_val(cmd, DELETE_LIB_ARG);

    /* first deactivate, than eventually activate. Deactivation has priority over activation */
    if (NULL != deactivate_arg) change_dissector_status(0, deactivate_arg->val, libs);
    if (NULL != activate_arg) change_dissector_status(1, activate_arg->val, libs);
    if (NULL != add_paths) add_and_load_dissectors(libs, custom_dissectors, add_paths);
    if (NULL != delete_filenames) delete_dissector(libs, delete_filenames);
    if (show_list) print_dissectors_list(libs);
    /* show_list should be the final command execution, because it shows the eventual previous changes */
}