#include "./file.h"

#include <SDL2/SDL.h>

#include "../logger/logger.h"
#include "../memory/memory.h"

ssize_t file_get_byte_size(const char* filename) {
    SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
    Sint64 res_size = rw->size(rw);
    if (res_size <= 0) {
        log_error("Unable to retrieve byte size of the file: %s %s", filename, SDL_GetError());
        return -1;
    }
    rw->close(rw);
    return res_size;
}

ssize_t file_read_binary(const char* filename, char* data) {
    SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
    Sint64 file_size = rw->size(rw);
    if (file_size <= 0) {
        log_error("Unable to read file: %s %s", filename, SDL_GetError());
        return -1;
    }
    char* buf = data;

    Sint64 total_bytes_read = 0, bytes_read = 1;
    while (total_bytes_read < file_size && bytes_read > 0) {
        Sint64 bytes_read = rw->read(rw, buf, 1, file_size - total_bytes_read);
        total_bytes_read += bytes_read;
        buf += bytes_read;
    }
    if (total_bytes_read != file_size) {
        log_error("Unable to read whole file: %ld / %ld", total_bytes_read, file_size);
        return -1;
    }
    rw->close(rw);

    return total_bytes_read;
}
