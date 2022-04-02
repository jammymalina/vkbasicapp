#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../core/string/string.h"
#include "./shader_types.h"

#define SHADER_MAX_BINDINGS 8

typedef struct Shader {
    VkShaderModule handle;
    ShaderType type;
} Shader;

static void shader_clear(Shader* shader) {
    shader->handle = VK_NULL_HANDLE;
    shader->type = SHADER_TYPE_UNDEFINED;
}

static void shader_copy(const Shader* src, Shader* dst) {
    dst->handle = src->handle;
    dst->type = src->type;
}

#endif
