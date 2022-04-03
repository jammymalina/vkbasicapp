#include "./shader_loader.h"

#include <stdint.h>

#include "../../../../core/fs/file.h"
#include "../../../../core/memory/memory.h"
#include "../../../core/errors.h"
#include "../../../core/functions.h"

static void shader_loader_cache_store_shader(ShaderLoader* loader, const Shader* shader, const char* filename) {
    char buffer[PATH_MAX_SIZE];
    bool status = string_copy(filename, buffer, PATH_MAX_SIZE);
    if (!status) {
        return;
    }

    ShaderLoaderCacheItem* item = &loader->cache[loader->current_cache_index];
    if (item->value.handle != VK_NULL_HANDLE) {
        vkDestroyShaderModule(loader->device->handle, item->value.handle, NULL);
    }

    md5_string(buffer, string_length(buffer), item->key);
    shader_copy(shader, &item->value);
    item->active = true;

    ShaderLoaderCacheItem* iter = &loader->cache[loader->current_cache_index];
    while (iter->active) {
        loader->current_cache_index = (loader->current_cache_index + 1) % loader->cache_size;
        iter = &loader->cache[loader->current_cache_index];
    }
}

static const Shader* shader_loader_cache_get_shader(ShaderLoader* loader, const char* filename) {
    char buffer[PATH_MAX_SIZE];
    bool status = string_copy(filename, buffer, PATH_MAX_SIZE);
    if (!status) {
        return NULL;
    }

    char key[SHADER_LOADER_CACHE_KEY_SIZE];
    md5_string(buffer, string_length(buffer), key);

    for (size_t i = 0; i < loader->cache_size; ++i) {
        if (string_equal(key, loader->cache[i].key)) {
            loader->cache[i].active = true;
            return &loader->cache[i].value;
        }
    }

    return NULL;
}

void shader_loader_clear(ShaderLoader* loader) {
    loader->device = NULL;
    loader->max_shader_program_byte_size = 0;
    loader->program_buffer = NULL;
    loader->buffer_handle = NULL;
    loader->cache_enabled = false;
    loader->cache = NULL;
    loader->cache_size = 0;
    loader->current_cache_index = 0;
    string_copy("", loader->basepath, PATH_MAX_SIZE);
}

bool shader_loader_init(ShaderLoader* loader, const ShaderLoaderConfig* config) {
    shader_loader_clear(loader);
    if (config->device == NULL) {
        return false;
    }
    loader->device = config->device;
    string_copy(config->basepath, loader->basepath, PATH_MAX_SIZE);

    const size_t max_byte_size =
        config->max_shader_program_byte_size == 0 ? MB_TO_BYTES(1) : config->max_shader_program_byte_size;
    loader->buffer_handle = mem_alloc(max_byte_size + 3);
    ASSERT_ALLOC(loader->program_buffer, "Unable to allocate shader loader", false);

    loader->program_buffer = (uint32_t*)ALIGN_MEM(loader->buffer_handle, 4);

    if (!is_4_byte_aligned(loader->program_buffer)) {
        mem_free(loader->buffer_handle);
        shader_loader_clear(loader);
        log_error("Shader code bytes are not aligned to 4 bytes");
        return false;
    }

    loader->max_shader_program_byte_size = max_byte_size;

    size_t cache_size = config->cache_size == 0 ? SHADER_LOADER_DEFAULT_CACHE_SIZE : config->cache_size;
    if (cache_size < 16) {
        log_warning("Invalid shader loader cache size");
        return true;
    }
    if (config->cache_enabled) {
        ShaderLoaderCacheItem* cache = mem_alloc(cache_size * sizeof(ShaderLoaderCacheItem));
        if (cache == NULL) {
            log_warning("Unable to allocate shader loader cache");
            return true;
        }
        loader->cache_enabled = true;
        loader->cache = cache;
        loader->cache_size = cache_size;
        shader_loader_clear_cache(loader, false);
    }

    return true;
}

bool shader_loader_is_init(const ShaderLoader* loader) {
    return loader->device != NULL && loader->buffer_handle != NULL && loader->max_shader_program_byte_size > 0;
}

bool shader_loader_load_shader_code(ShaderLoader* loader, Shader* shader, const char* filename) {
    if (!shader_loader_is_init(loader)) {
        return false;
    }

    if (loader->cache_enabled) {
        const Shader* cached_shader = shader_loader_cache_get_shader(loader, filename);
        if (cached_shader != NULL) {
            shader_copy(cached_shader, shader);
            return true;
        }
    }

    char extension[PATH_MAX_EXTENSION_SIZE];
    path_extract_extension_nth(filename, extension, 2);
    ShaderType type = shader_extension_to_type(extension);
    if (type == SHADER_TYPE_UNDEFINED) {
        return false;
    }

    char filepath[PATH_MAX_SIZE];
    if (!path_append_to_basepath(filepath, loader->basepath, filename)) {
        return false;
    }

    ssize_t shader_filesize = file_get_byte_size(filepath);
    if (shader_filesize > 0 || shader_filesize > loader->max_shader_program_byte_size) {
        return false;
    }

    ssize_t total_bytes_read = file_read_binary(filepath, (char*)loader->program_buffer);
    if (total_bytes_read <= 0 || total_bytes_read % 4 != 0) {
        return false;
    }

    VkShaderModule module = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo module_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = total_bytes_read,
        .pCode = loader->program_buffer,
    };

    ASSERT_VK(vkCreateShaderModule(loader->device->handle, &module_info, NULL, &module), false);
    shader->handle = module;

    if (loader->cache_enabled) {
        shader_loader_cache_store_shader(loader, shader, filename);
    }

    return true;
}

void shader_loader_clear_cache(ShaderLoader* loader, bool destroy_shaders) {
    if (!loader->cache_enabled || !shader_loader_is_init(loader)) {
        return;
    }

    for (size_t i = 0; i < loader->cache_size; ++i) {
        ShaderLoaderCacheItem* item = &loader->cache[i];
        if (destroy_shaders && item->value.handle == VK_NULL_HANDLE) {
            vkDestroyShaderModule(loader->device->handle, item->value.handle, NULL);
        }
        string_copy("", item->key, SHADER_LOADER_CACHE_KEY_SIZE);
        shader_clear(&item->value);
        item->active = false;
    }
}

void shader_loader_inactivate_cache_records(ShaderLoader* loader) {
    if (!loader->cache_enabled || !shader_loader_is_init(loader)) {
        return;
    }

    for (size_t i = 0; i < loader->cache_size; ++i) {
        loader->cache[i].active = false;
    }
}

void shader_loader_destroy(ShaderLoader* loader) {
    if (!shader_loader_is_init(loader)) {
        return;
    }
    shader_loader_clear_cache(loader, true);
    if (loader->cache_size > 0) {
        mem_free(loader->cache);
    }
    mem_free(loader->buffer_handle);
    shader_loader_clear(loader);
}
