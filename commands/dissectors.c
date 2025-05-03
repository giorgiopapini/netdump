#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <string.h>

#include "dissectors.h"
#include "../status_handler.h"
#include "../utils/formats.h"
#include "../utils/colors.h"
#include "../utils/string_utils.h"


void add_and_load_single_dissector(shared_libs *libs, custom_dissectors *dissectors, char *path);
void add_and_load_dissectors_dir(shared_libs *libs, custom_dissectors *dissectors, char *dir_path);

void change_dissector_status(int new_status, char *filenames, shared_libs *libs) {
    char *token;
    char *trimmed;
    int found = 0;
    size_t i;

    if (NULL == libs) {
        print_warning_msg(DISSECTORS_EMPTY_WARNING);
        return;
    }

    if (NULL == filenames) {
        for (i = 0; i < libs->count; i ++) libs->statuses[i] = new_status;
        if (0 == new_status) print_success_msg(DISSECTOR_DEACTIVATED_SUCCESS);
        else print_success_msg(DISSECTOR_ACTIVATED_SUCCESS);
        return;
    }

    token = strtok(filenames, STRINGS_SEPARATOR);
    while (token != NULL) {
        found = 0;
        trimmed = get_trimmed_str(token);

        if (NULL != libs->filenames) {
            for (i = 0; i < libs->count; i ++) {
                if (NULL != libs->filenames[i] && 0 == strcmp(trimmed, libs->filenames[i])) {
                    found = 1;
                    break;
                }
            }
        }

        if (!found) raise_error(LIB_NOT_FOUND_ERROR, 0, NULL, trimmed);
        else {
            if (0 == new_status) print_success_msg(DISSECTOR_DEACTIVATED_SUCCESS);
            else print_success_msg(DISSECTOR_ACTIVATED_SUCCESS);
            libs->statuses[i] = new_status;
        }

        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
}

void add_and_load_single_dissector(shared_libs *libs, custom_dissectors *dissectors, char *path) {
    char *filename;
    void *handle;

    filename = get_filename(path);
    if (!has_shared_lib_ext((const char *)filename)) return;  /* ADD MESSAGE ERROR! */

    handle = dlopen(path, RTLD_LAZY);
    if (NULL != handle) {
        add_shared_lib(libs, handle, strdup(filename), 1);
        load_dissector(dissectors, handle, strdup(filename));
        print_success_msg(DISSECTOR_LOADED_SUCCESS);
    }
    else raise_error(LOADING_SHARED_LIB_ERROR, 0, NULL, filename, dlerror());
}

void add_and_load_dissectors_dir(shared_libs *libs, custom_dissectors *dissectors, char *dir_path) {
    struct dirent *entry;
    char full_path[PATH_MAX];
    DIR *dir;
    
    if (NULL == dir_path) return;

    dir = opendir(dir_path);
    if (!dir) {
        raise_error(DIR_OPEN_ERROR, 0, NULL, dir_path);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG || entry->d_type == DT_LNK || entry->d_type == DT_UNKNOWN) {
            if (has_shared_lib_ext(entry->d_name)) {
                if (dir_path[strlen(dir_path) - 1] == '/')
                    snprintf(full_path, sizeof(full_path), "%s%s", dir_path, entry->d_name);
                else
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
                add_and_load_single_dissector(libs, dissectors, full_path);
            }
        }
    }

    closedir(dir);
}

void add_and_load_dissectors(shared_libs *libs, custom_dissectors *dissectors, char *paths) {
    struct stat path_stat;
    char *token;
    char *trimmed;

    if (NULL == paths) return;

    token = strtok(paths, STRINGS_SEPARATOR);
    while (token != NULL) {
        trimmed = get_trimmed_str(token);

        if (0 != stat(trimmed, &path_stat)) {
            raise_error(NO_SUCH_FILE_OR_DIR_ERROR, 0, NULL, trimmed);
            return;
        }

        if (S_ISDIR(path_stat.st_mode)) add_and_load_dissectors_dir(libs, dissectors, trimmed);
        else if (S_ISREG(path_stat.st_mode)) add_and_load_single_dissector(libs, dissectors, trimmed);

        token = strtok(NULL, STRINGS_SEPARATOR);
        free(trimmed);
    }
}

void print_dissectors_list(shared_libs *libs) {
    int empty = 1;
    size_t i;

    if (NULL == libs) {
        print_warning_msg(DISSECTORS_EMPTY_WARNING);
        return;
    }

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

void execute_dissectors(command *cmd, shared_libs *libs, custom_dissectors *custom_diss) {
    int show_list = (NULL != get_arg(cmd, DISSECTOR_LIST_ARG) || 0 == cmd->n_hashes);
    arg *activate_arg = get_arg(cmd, ACTIVATE_LIB_ARG);
    arg *deactivate_arg = get_arg(cmd, DEACTIVATE_LIB_ARG);
    char *add_paths = get_raw_val(cmd, ADD_DISSECTOR_ARG);

    /* first deactivate, than eventually activate. Deactivation has priority over activation */
    if (NULL != deactivate_arg) change_dissector_status(0, deactivate_arg->val, libs);
    if (NULL != activate_arg) change_dissector_status(1, activate_arg->val, libs);
    if (NULL != add_paths) add_and_load_dissectors(libs, custom_diss, add_paths);
    if (show_list) print_dissectors_list(libs);
    /* show_list should be the final command execution, because it shows the eventual previous changes */
}