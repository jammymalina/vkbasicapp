#ifndef FS_FILE_H
#define FS_FILE_H

#include <sys/types.h>

ssize_t file_get_byte_size(const char* filename);
ssize_t file_read_binary(const char* filename, char* data);

#endif
