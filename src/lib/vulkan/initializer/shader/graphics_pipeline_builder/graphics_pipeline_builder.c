#include "./graphics_pipeline_builder.h"

#include "../../../core/functions.h"
#include "../../../utils/debug.h"

static bool graphics_pipeline_builder_validate(const GraphicsPipelineBuilder* builder) {
    if (builder->vertex_layout_type == VERTEX_LAYOUT_UNKNOWN || builder->vertex_layout_type == VERTEX_LAYOUTS_TOTAL) {
        log_warning("Graphics pipeline builder validation failed - invalid vertex layout");
        return false;
    }

    if (builder->shader_file_count == 0 || builder->shader_files == NULL) {
        log_warning("Graphics pipeline builder validation failed - no shader files provided");
        return false;
    }

    return true;
}

static void graphics_pipeline_builder_clear_shaders(GraphicsPipelineBuilder* builder, bool destroy_shaders) {
    for (size_t i = 0; i < SHADER_TYPES_TOTAL; ++i) {
        if (destroy_shaders && builder->shaders[i].handle != VK_NULL_HANDLE) {
            vkDestroyShaderModule(builder->device->handle, builder->shaders[i].handle, NULL);
        }
        shader_clear(&builder->shaders[i]);
    }
}

void graphics_pipeline_builder_clear(GraphicsPipelineBuilder* builder) {
    builder->device = NULL;
    builder->render_state_flags = 0;
    builder->pipeline_cache = VK_NULL_HANDLE;
    shader_loader_clear(&builder->shader_loader);
    graphics_pipeline_builder_clear_shaders(builder, false);
    graphics_pipeline_builder_reset_defaults(builder);
}

bool graphics_pipeline_builder_init(GraphicsPipelineBuilder* builder, const GraphicsPipelineBuilderConfig* config) {
    graphics_pipeline_builder_clear(builder);

    if (config->device == NULL) {
        return false;
    }

    ShaderLoaderConfig loader_config = {
        .device = config->device,
        .max_shader_program_byte_size = config->shader_buffer_size,
        .cache_enabled = config->shader_cache_enabled,
        .cache_size = config->shader_cache_size,
        .basepath = config->basepath,
    };
    bool status = shader_loader_init(&builder->shader_loader, &loader_config);
    if (!status) {
        return false;
    }

    if (config->pipeline_cache_enabled) {
        VkPipelineCache cache_handle;
        VkPipelineCacheCreateInfo cache_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .initialDataSize = 0,
            .pInitialData = NULL,
        };
        VkResult cache_status = vkCreatePipelineCache(builder->device->handle, &cache_info, NULL, &cache_handle);
        if (cache_status == VK_SUCCESS) {
            builder->pipeline_cache = cache_handle;
        } else {
            log_warning("Unable to create pipeline cache %s", vulkan_result_to_string(cache_status));
        }
    }

    return true;
}

bool graphics_pipeline_builder_is_init(const GraphicsPipelineBuilder* builder) {
    return builder->device != NULL && shader_loader_is_init(&builder->shader_loader);
}

void graphics_pipeline_builder_reset_defaults(GraphicsPipelineBuilder* builder) {
    builder->render_state_flags = RST_BASIC_3D;
    builder->vertex_layout_type = VERTEX_LAYOUT_POS_NOR_UV;
    builder->shader_files = NULL;
    builder->shader_file_count = 0;
}

void graphics_pipeline_builder_set_shader_files(
    GraphicsPipelineBuilder* builder, const char** shader_files, size_t shader_file_count) {
    builder->shader_files = shader_files;
    builder->shader_file_count = shader_file_count;
}

void graphics_pipeline_builder_start(GraphicsPipelineBuilder* builder) {
    if (!graphics_pipeline_builder_is_init(builder)) {
        return;
    }

    shader_loader_inactivate_cache_records(&builder->shader_loader);
    // if caching of shader modules is enabled we don't want to destroy shaders
    graphics_pipeline_builder_clear_shaders(builder, !builder->shader_loader.cache_enabled);
}

bool graphics_pipeline_builder_build(GraphicsPipelineBuilder* builder, GraphicsPipeline* pipeline) {
    if (!graphics_pipeline_builder_is_init(builder)) {
        return false;
    }

    if (!graphics_pipeline_builder_validate(builder)) {
        return false;
    }

    return true;
}

void graphics_pipeline_builder_destroy(GraphicsPipelineBuilder* builder) {
    if (!graphics_pipeline_builder_is_init(builder)) {
        return;
    }
    shader_loader_destroy(&builder->shader_loader);
    graphics_pipeline_builder_clear(builder);
}
