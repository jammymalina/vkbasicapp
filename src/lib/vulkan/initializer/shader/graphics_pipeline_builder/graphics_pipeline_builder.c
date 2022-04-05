#include "./graphics_pipeline_builder.h"

#include "../../../core/errors.h"
#include "../../../core/functions.h"
#include "../../../core/vertex/vertex_layout.h"
#include "../../../utils/debug.h"
#include "../render_state_transformer/render_state_transformer.h"

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

static bool graphics_pipeline_builder_init_shaders(GraphicsPipelineBuilder* builder) {
    for (size_t i = 0; i < builder->shader_file_count; ++i) {
        Shader shader;
        bool status = shader_loader_load_shader_code(&builder->shader_loader, &shader, builder->shader_files[i]);
        if (!status) {
            log_error("Unable to load the shader: %s", builder->shader_files[i]);
            return false;
        }
        shader_copy(&shader, &builder->shaders[shader_type_to_index(shader.type)]);
    }

    return true;
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
    builder->device = config->device;

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
    builder->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    builder->shader_files = NULL;
    builder->shader_file_count = 0;

    builder->color_attachment_count = 0;
    builder->color_attachments = NULL;
    builder->depth_attachment_format = VK_FORMAT_UNDEFINED;
    builder->stencil_attachment_format = VK_FORMAT_UNDEFINED;
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

    bool status = graphics_pipeline_builder_init_shaders(builder);
    if (!status) {
        return false;
    }

    // vertex layout
    const VertexLayout* vertex_layouts = vertex_layout_get_all();
    const VertexLayout* vertex_layout = &vertex_layouts[builder->vertex_layout_type];

    VkPipelineVertexInputStateCreateInfo vertex_input_state = vertex_layout->input_state;
    vertex_input_state.vertexBindingDescriptionCount = vertex_layout->binding_description_count;
    vertex_input_state.pVertexBindingDescriptions = vertex_layout->binding_descriptions;
    vertex_input_state.vertexAttributeDescriptionCount = vertex_layout->attribute_description_count;
    vertex_input_state.pVertexAttributeDescriptions = vertex_layout->attribute_descriptions;

    // input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .topology = builder->topology,
        .primitiveRestartEnable = VK_FALSE,
    };

    // rasterization
    VkPipelineRasterizationStateCreateInfo rasterization_state =
        render_state_transformer_get_rasterization_state(builder->render_state_flags);

    // color blend attachment
    VkPipelineColorBlendAttachmentState color_blend_attachment =
        render_state_transformer_get_color_blend_attachment(builder->render_state_flags);

    // color blend
    VkPipelineColorBlendStateCreateInfo color_blend = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_CLEAR,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
        .blendConstants = {0, 0, 0, 0},
    };

    // depth / stencil
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info =
        render_state_transformer_get_depth_stencil_state(builder->render_state_flags, builder->device);

    // multisample
    VkPipelineMultisampleStateCreateInfo multisample_info =
        render_state_transformer_get_multisample_state(builder->render_state_flags);

    // create shader modules
    VkPipelineShaderStageCreateInfo shader_stages[SHADER_TYPES_TOTAL];
    size_t shader_stage_count = 0;

    for (size_t i = 0; i < SHADER_TYPES_TOTAL; ++i) {
        if (builder->shaders[i].handle == VK_NULL_HANDLE) {
            continue;
        }

        const Shader* shader = &builder->shaders[i];

        VkPipelineShaderStageCreateInfo shader_stage_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .stage = shader_type_to_stage(shader->type),
            .module = shader->handle,
            .pName = "main",
            .pSpecializationInfo = NULL,
        };
        shader_stages[shader_stage_count] = shader_stage_info;
        ++shader_stage_count;
    }

    // dynamic states
    VkDynamicState dynamic_states[RENDER_STATE_MAX_DYNAMIC_STATES];
    size_t dynamic_state_count = 0;
    render_state_transformer_get_dynamic_states(
        builder->render_state_flags, builder->device, dynamic_states, &dynamic_state_count);

    VkPipelineDynamicStateCreateInfo dynamic_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .dynamicStateCount = dynamic_state_count,
        .pDynamicStates = dynamic_states,
    };

    // pViewports and pScissors ignored because of dynamic state
    VkPipelineViewportStateCreateInfo viewport_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = NULL,
        .scissorCount = 1,
        .pScissors = NULL,
    };

    VkPipelineLayout pipeline_layout;
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = NULL,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };
    VkResult pipeline_status =
        vkCreatePipelineLayout(builder->device->handle, &pipeline_layout_info, NULL, &pipeline_layout);
    ASSERT_VK_LOG(pipeline_status, "Unable to create pipeline layout", false);

    // pipeline
    const VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .pNext = NULL,
        .viewMask = 0,
        .colorAttachmentCount = builder->color_attachment_count,
        .pColorAttachmentFormats = builder->color_attachments,
        .depthAttachmentFormat = builder->depth_attachment_format,
        .stencilAttachmentFormat = builder->stencil_attachment_format,
    };

    VkPipeline graphics_pipeline;
    VkGraphicsPipelineCreateInfo pipeline_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipeline_rendering_create_info,
        .flags = 0,
        .stageCount = shader_stage_count,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_state,
        .pInputAssemblyState = &input_assembly,
        .pTessellationState = NULL,
        .pViewportState = &viewport_info,
        .pRasterizationState = &rasterization_state,
        .pMultisampleState = &multisample_info,
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend,
        .pDynamicState = &dynamic_info,
        .layout = pipeline_layout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };
    pipeline_status = vkCreateGraphicsPipelines(
        builder->device->handle, builder->pipeline_cache, 1, &pipeline_info, NULL, &graphics_pipeline);
    ASSERT_VK_LOG(pipeline_status, "Unable to create pipeline", false);

    pipeline->device = builder->device;
    pipeline->handle = graphics_pipeline;
    pipeline->layout = pipeline_layout;

    return true;
}

void graphics_pipeline_builder_destroy(GraphicsPipelineBuilder* builder) {
    shader_loader_destroy(&builder->shader_loader);
    graphics_pipeline_builder_clear(builder);
}
