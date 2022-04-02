#ifndef FS_PATH_H
#define FS_PATH_H

#include <stdbool.h>
#include <stddef.h>

#define PATH_MAX_SIZE 4096
#define PATH_MAX_EXTENSION_SIZE 32

bool path_get_basepath(char basepath[PATH_MAX_SIZE]);
bool path_append_to_basepath(
    char dst[PATH_MAX_SIZE], const char basepath[PATH_MAX_SIZE], const char filepath[PATH_MAX_SIZE]);

bool path_extract_extension(const char path[PATH_MAX_SIZE], char extension[PATH_MAX_EXTENSION_SIZE]);
bool path_extract_extension_nth(const char path[PATH_MAX_SIZE], char extension[PATH_MAX_EXTENSION_SIZE], size_t n);

#endif
