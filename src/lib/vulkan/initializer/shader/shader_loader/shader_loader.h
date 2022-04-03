#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../../core/fs/path.h"
#include "../../../../core/utils/md5/md5.h"
#include "../../../core/device/device.h"
#include "../../../core/shader/shader.h"

#define SHADER_LOADER_CACHE_KEY_SIZE MD5_DIGEST_STRING_LENGTH
#define SHADER_LOADER_DEFAULT_CACHE_SIZE 64

typedef struct ShaderLoaderCacheItem {
    bool active;
    Shader value;
    char key[SHADER_LOADER_CACHE_KEY_SIZE];
} ShaderLoaderCacheItem;

typedef struct ShaderLoaderConfig {
    const Device* device;
    const char* basepath;

    size_t max_shader_program_byte_size;
    bool cache_enabled;
    size_t cache_size;
} ShaderLoaderConfig;

typedef struct ShaderLoader {
    const Device* device;
    char basepath[PATH_MAX_SIZE];

    void* buffer_handle;
    uint32_t* program_buffer;
    size_t max_shader_program_byte_size;

    bool cache_enabled;
    ShaderLoaderCacheItem* cache;
    size_t cache_size;
    size_t current_cache_index;
} ShaderLoader;

void shader_loader_clear(ShaderLoader* loader);
bool shader_loader_init(ShaderLoader* loader, const ShaderLoaderConfig* config);
bool shader_loader_is_init(const ShaderLoader* loader);

bool shader_loader_load_shader_code(ShaderLoader* loader, Shader* shader, const char* filename);
void shader_loader_clear_cache(ShaderLoader* loader, bool destroy_shaders);
void shader_loader_inactivate_cache_records(ShaderLoader* loader);

void shader_loader_destroy(ShaderLoader* loader);

#endif
