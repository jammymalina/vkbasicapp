#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../core/string/string.h"

typedef enum ShaderType {
    SHADER_TYPE_VERTEX = 1,
    SHADER_TYPE_FRAGMENT = 2,
    SHADER_TYPE_GEOMETRY = 4,
    SHADER_TYPE_COMPUTE = 8,
    SHADER_TYPE_TESS_CTRL = 16,
    SHADER_TYPE_TESS_EVAL = 32,
    SHADER_TYPE_UNDEFINED = 64
} ShaderType;

typedef enum ShaderProgramType {
    SHADER_PROGRAM_TYPE_UNDEFINED = -1,
    SHADER_PROGRAM_TYPE_GRAPHICS,
    SHADER_PROGRAM_TYPE_COMPUTE,
    SHADER_PROGRAM_TYPES_TOTAL
} ShaderProgramType;

#define SHADER_TYPE_GROUP_GRAPHICS (SHADER_TYPE_VERTEX | SHADER_TYPE_FRAGMENT)

typedef enum ShaderBindingType {
    SHADER_BINDING_TYPE_UNDEFINED = -1,
    SHADER_BINDING_TYPE_UNIFORM,
    SHADER_BINDING_TYPE_SAMPLER,
    SHADER_BINDINGS_COUNT
} ShaderBindingType;

typedef struct ShaderBinding {
    ShaderBindingType type;
    uint32_t binding;
} ShaderBinding;

static inline ShaderType shader_extension_to_type(const char* extension) {
    if (string_equal(extension, "vert")) return SHADER_TYPE_VERTEX;
    if (string_equal(extension, "frag")) return SHADER_TYPE_FRAGMENT;
    if (string_equal(extension, "tesc")) return SHADER_TYPE_TESS_CTRL;
    if (string_equal(extension, "tese")) return SHADER_TYPE_TESS_EVAL;
    if (string_equal(extension, "geom")) return SHADER_TYPE_GEOMETRY;
    if (string_equal(extension, "comp")) return SHADER_TYPE_COMPUTE;

    return SHADER_TYPE_UNDEFINED;
}

static inline const char* shader_type_to_extension(ShaderType type) {
    switch (type) {
        case SHADER_TYPE_VERTEX:
            return "vert";
        case SHADER_TYPE_FRAGMENT:
            return "frag";
        case SHADER_TYPE_TESS_CTRL:
            return "tesc";
        case SHADER_TYPE_TESS_EVAL:
            return "tese";
        case SHADER_TYPE_GEOMETRY:
            return "geom";
        case SHADER_TYPE_COMPUTE:
            return "comp";
        default:
            return "undefined";
    }
}

static inline VkShaderStageFlagBits shader_type_to_stage(ShaderType type) {
    switch (type) {
        case SHADER_TYPE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case SHADER_TYPE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case SHADER_TYPE_TESS_CTRL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case SHADER_TYPE_TESS_EVAL:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case SHADER_TYPE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SHADER_TYPE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_ALL;
    }
}

static inline VkDescriptorType shader_binding_type_to_descriptor_type(ShaderBindingType type) {
    switch (type) {
        case SHADER_BINDING_TYPE_UNIFORM:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case SHADER_BINDING_TYPE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
    }
}

#endif
