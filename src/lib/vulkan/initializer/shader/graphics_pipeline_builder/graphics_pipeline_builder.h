#ifndef GRAPHICS_PIPELINE_BUILDER_H
#define GRAPHICS_PIPELINE_BUILDER_H

#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../../../../core/memory/memory.h"
#include "../../../core/device/device.h"
#include "../../../core/rendering/render_state_bits.h"
#include "../../../core/shader/graphics_pipeline.h"
#include "../../../core/shader/shader_types.h"
#include "../../../core/vertex/vertex_layout.h"
#include "../shader_loader/shader_loader.h"

typedef struct GraphicsPipelineBuilderConfig {
    const Device* device;
    const char* basepath;

    bool shader_cache_enabled;
    size_t shader_cache_size;
    size_t shader_buffer_size;

    bool pipeline_cache_enabled;
} GraphicsPipelineBuilderConfig;

static inline GraphicsPipelineBuilderConfig graphics_pipeline_builder_get_default_config() {
    return (GraphicsPipelineBuilderConfig){
        .device = NULL,
        .basepath = "",
        .shader_cache_enabled = true,
        .shader_cache_size = 256,
        .shader_buffer_size = MB_TO_BYTES(1),
        .pipeline_cache_enabled = false,
    };
}

typedef struct GraphicsPipelineBuilder {
    const Device* device;

    Shader shaders[SHADER_TYPES_TOTAL];

    RenderStateFlags render_state_flags;
    VertexLayoutType vertex_layout_type;
    VkPrimitiveTopology topology;
    const char** shader_files;
    size_t shader_file_count;

    uint32_t color_attachment_count;
    const VkFormat* color_attachments;
    VkFormat depth_attachment_format;
    VkFormat stencil_attachment_format;

    ShaderLoader shader_loader;
    VkPipelineCache pipeline_cache;
} GraphicsPipelineBuilder;

void graphics_pipeline_builder_clear(GraphicsPipelineBuilder* builder);
bool graphics_pipeline_builder_init(GraphicsPipelineBuilder* builder, const GraphicsPipelineBuilderConfig* config);
bool graphics_pipeline_builder_is_init(const GraphicsPipelineBuilder* builder);

void graphics_pipeline_builder_reset_defaults(GraphicsPipelineBuilder* builder);
void graphics_pipeline_builder_set_shader_files(
    GraphicsPipelineBuilder* builder, const char** shader_files, size_t shader_file_count);

void graphics_pipeline_builder_start(GraphicsPipelineBuilder* builder);
bool graphics_pipeline_builder_build(GraphicsPipelineBuilder* builder, GraphicsPipeline* pipeline);

void graphics_pipeline_builder_destroy(GraphicsPipelineBuilder* builder);

#endif
