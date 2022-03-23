#include "./memory.h"

#include <SDL2/SDL.h>

void* mem_alloc(size_t size) { return SDL_malloc(size); }

void* mem_realloc(void* mem, size_t size) { return SDL_realloc(mem, size); }

void mem_free(void* data) { SDL_free(data); }

void mem_copy(void* dest, const void* src, size_t length) { SDL_memcpy(dest, src, length); }

void* mem_move(void* dest, const void* src, size_t length) { return SDL_memmove(dest, src, length); }

int mem_cmp(const void* m1, const void* m2, size_t length) { return SDL_memcmp(m1, m2, length); }

void mem_set(void* m1, int value, size_t length) { SDL_memset(m1, value, length); }
