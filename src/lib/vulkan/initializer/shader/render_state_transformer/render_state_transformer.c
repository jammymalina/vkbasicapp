#include "./render_state_transformer.h"

#include <stdbool.h>

static inline VkCullModeFlagBits render_state_transformer_get_cull_mode(RenderStateFlags flags) {
    switch (flags & RST_CULL_BITS) {
        case RST_CULL_TWOSIDED:
            return VK_CULL_MODE_NONE;
        case RST_CULL_BACKSIDED:
            return flags & RST_MIRROR_VIEW ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_BACK_BIT;
        case RST_CULL_FRONTSIDED:
            return flags & RST_MIRROR_VIEW ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_FRONT_BIT;
        default:
            return VK_CULL_MODE_NONE;
    }
}

static inline VkBlendFactor render_state_transformer_get_src_blend_factor(RenderStateFlags flags) {
    switch (flags & RST_BLENDOP_BITS) {
        case RST_SRCBLEND_ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case RST_SRCBLEND_ONE:
            return VK_BLEND_FACTOR_ONE;
        case RST_SRCBLEND_DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        case RST_SRCBLEND_ONE_MINUS_DST_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case RST_SRCBLEND_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case RST_SRCBLEND_ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case RST_SRCBLEND_DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case RST_SRCBLEND_ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:
            return VK_BLEND_FACTOR_ONE;
    }
}

static inline VkBlendFactor render_state_transformer_get_dst_blend_factor(RenderStateFlags flags) {
    switch (flags & RST_DSTBLEND_BITS) {
        case RST_DSTBLEND_ZERO:
            return VK_BLEND_FACTOR_ZERO;
        case RST_DSTBLEND_ONE:
            return VK_BLEND_FACTOR_ONE;
        case RST_DSTBLEND_SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case RST_DSTBLEND_ONE_MINUS_SRC_COLOR:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case RST_DSTBLEND_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case RST_DSTBLEND_ONE_MINUS_SRC_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case RST_DSTBLEND_DST_ALPHA:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case RST_DSTBLEND_ONE_MINUS_DST_ALPHA:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:
            return VK_BLEND_FACTOR_ZERO;
    }
}

static inline VkBlendOp render_state_transformer_get_blend_op(RenderStateFlags flags) {
    switch (flags & RST_BLENDOP_BITS) {
        case RST_BLENDOP_MIN:
            return VK_BLEND_OP_MIN;
        case RST_BLENDOP_MAX:
            return VK_BLEND_OP_MAX;
        case RST_BLENDOP_ADD:
            return VK_BLEND_OP_ADD;
        case RST_BLENDOP_SUB:
            return VK_BLEND_OP_SUBTRACT;
        default:
            return VK_BLEND_OP_ADD;
    }
}

static inline VkColorComponentFlags render_state_transformer_get_color_mask(RenderStateFlags flags) {
    VkColorComponentFlags color_mask = 0;

    color_mask |= flags & RST_REDMASK ? 0 : VK_COLOR_COMPONENT_R_BIT;
    color_mask |= flags & RST_GREENMASK ? 0 : VK_COLOR_COMPONENT_G_BIT;
    color_mask |= flags & RST_BLUEMASK ? 0 : VK_COLOR_COMPONENT_B_BIT;
    color_mask |= flags & RST_ALPHAMASK ? 0 : VK_COLOR_COMPONENT_A_BIT;

    return color_mask;
}

static inline VkCompareOp render_state_transformer_get_depth_compare_op(RenderStateFlags flags) {
    switch (flags & RST_DEPTHFUNC_BITS) {
        case RST_DEPTHFUNC_EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case RST_DEPTHFUNC_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        case RST_DEPTHFUNC_LESS:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case RST_DEPTHFUNC_GREATER:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        default:
            return VK_COMPARE_OP_ALWAYS;
    }
}

static inline VkCompareOp render_state_transformer_get_stencil_compare_op(RenderStateFlags flags) {
    switch (flags & RST_STENCIL_FUNC_BITS) {
        case RST_STENCIL_FUNC_NEVER:
            return VK_COMPARE_OP_NEVER;
        case RST_STENCIL_FUNC_LESS:
            return VK_COMPARE_OP_LESS;
        case RST_STENCIL_FUNC_EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case RST_STENCIL_FUNC_LEQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case RST_STENCIL_FUNC_GREATER:
            return VK_COMPARE_OP_GREATER;
        case RST_STENCIL_FUNC_NOTEQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case RST_STENCIL_FUNC_GEQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case RST_STENCIL_FUNC_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_ALWAYS;
    }
}

static inline VkStencilOp render_state_transformer_get_stencil_fail_op(uint64_t stencil_bits) {
    switch (stencil_bits & RST_STENCIL_OP_FAIL_BITS) {
        case RST_STENCIL_OP_FAIL_KEEP:
            return VK_STENCIL_OP_KEEP;
        case RST_STENCIL_OP_FAIL_ZERO:
            return VK_STENCIL_OP_ZERO;
        case RST_STENCIL_OP_FAIL_REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case RST_STENCIL_OP_FAIL_INCR:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case RST_STENCIL_OP_FAIL_DECR:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case RST_STENCIL_OP_FAIL_INVERT:
            return VK_STENCIL_OP_INVERT;
        case RST_STENCIL_OP_FAIL_INCR_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case RST_STENCIL_OP_FAIL_DECR_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            return VK_STENCIL_OP_KEEP;
    }
}

static inline VkStencilOp render_state_transformer_get_stencil_depth_fail_op(uint64_t stencil_bits) {
    switch (stencil_bits & RST_STENCIL_OP_ZFAIL_BITS) {
        case RST_STENCIL_OP_ZFAIL_KEEP:
            return VK_STENCIL_OP_KEEP;
        case RST_STENCIL_OP_ZFAIL_ZERO:
            return VK_STENCIL_OP_ZERO;
        case RST_STENCIL_OP_ZFAIL_REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case RST_STENCIL_OP_ZFAIL_INCR:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case RST_STENCIL_OP_ZFAIL_DECR:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case RST_STENCIL_OP_ZFAIL_INVERT:
            return VK_STENCIL_OP_INVERT;
        case RST_STENCIL_OP_ZFAIL_INCR_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case RST_STENCIL_OP_ZFAIL_DECR_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            return VK_STENCIL_OP_KEEP;
    }
}

static inline VkStencilOp render_state_transformer_get_stencil_pass_op(uint64_t stencil_bits) {
    switch (stencil_bits & RST_STENCIL_OP_PASS_BITS) {
        case RST_STENCIL_OP_PASS_KEEP:
            return VK_STENCIL_OP_KEEP;
        case RST_STENCIL_OP_PASS_ZERO:
            return VK_STENCIL_OP_ZERO;
        case RST_STENCIL_OP_PASS_REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case RST_STENCIL_OP_PASS_INCR:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case RST_STENCIL_OP_PASS_DECR:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case RST_STENCIL_OP_PASS_INVERT:
            return VK_STENCIL_OP_INVERT;
        case RST_STENCIL_OP_PASS_INCR_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case RST_STENCIL_OP_PASS_DECR_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            return VK_STENCIL_OP_KEEP;
    }
}

static inline VkStencilOpState render_state_transformer_get_stencil_op_state(uint64_t stencil_bits) {
    VkStencilOp fail_op = render_state_transformer_get_stencil_fail_op(stencil_bits);
    VkStencilOp pass_op = render_state_transformer_get_stencil_pass_op(stencil_bits);
    VkStencilOp depth_fail_op = render_state_transformer_get_stencil_depth_fail_op(stencil_bits);

    VkStencilOpState state = {
        .failOp = fail_op,
        .passOp = pass_op,
        .depthFailOp = depth_fail_op,
        .compareOp = VK_COMPARE_OP_NEVER,
        .compareMask = 0,
        .writeMask = 0,
        .reference = 0,
    };

    return state;
}

static inline VkStencilOpState render_state_transformer_get_front_stencil(RenderStateFlags flags) {
    VkCompareOp stencil_compare_op = render_state_transformer_get_stencil_compare_op(flags);

    uint32_t ref = (((uint32_t)(flags & RST_STENCIL_FUNC_REF_BITS)) >> RST_STENCIL_FUNC_REF_SHIFT);
    uint32_t mask = (((uint32_t)(flags & RST_STENCIL_FUNC_MASK_BITS)) >> RST_STENCIL_FUNC_MASK_SHIFT);
    uint64_t stencil_bits = flags & RST_SEPARATE_STENCIL ? flags & RST_STENCIL_FRONT_OPS : flags;

    VkStencilOpState front_stencil = render_state_transformer_get_stencil_op_state(stencil_bits);
    front_stencil.writeMask = 0xFFFFFFFF;
    front_stencil.compareOp = stencil_compare_op;
    front_stencil.compareMask = mask;
    front_stencil.reference = ref;

    return front_stencil;
}

static inline VkStencilOpState render_state_transformer_get_back_stencil(RenderStateFlags flags) {
    VkCompareOp stencil_compare_op = render_state_transformer_get_stencil_compare_op(flags);

    uint32_t ref = (((uint32_t)(flags & RST_STENCIL_FUNC_REF_BITS)) >> RST_STENCIL_FUNC_REF_SHIFT);
    uint32_t mask = (((uint32_t)(flags & RST_STENCIL_FUNC_MASK_BITS)) >> RST_STENCIL_FUNC_MASK_SHIFT);
    uint64_t stencil_bits = flags & RST_SEPARATE_STENCIL ? (flags & RST_STENCIL_BACK_OPS) >> 12 : flags;

    VkStencilOpState back_stencil = render_state_transformer_get_stencil_op_state(stencil_bits);
    back_stencil.writeMask = 0xFFFFFFFF;
    back_stencil.compareOp = stencil_compare_op;
    back_stencil.compareMask = mask;
    back_stencil.reference = ref;

    return back_stencil;
}

VkPipelineRasterizationStateCreateInfo render_state_transformer_get_rasterization_state(RenderStateFlags flags) {
    VkCullModeFlagBits cull_mode = render_state_transformer_get_cull_mode(flags);

    return (VkPipelineRasterizationStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = cull_mode,
        .frontFace = flags & RST_CLOCKWISE ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = (flags & RST_POLYGON_OFFSET) != 0,
        .depthBiasConstantFactor = 0,
        .depthBiasClamp = 0,
        .depthBiasSlopeFactor = 0,
        .lineWidth = 1.0f,
    };
}

VkPipelineColorBlendAttachmentState render_state_transformer_get_color_blend_attachment(RenderStateFlags flags) {
    VkBlendFactor src_factor = render_state_transformer_get_src_blend_factor(flags);
    VkBlendFactor dst_factor = render_state_transformer_get_dst_blend_factor(flags);
    VkBlendOp blend_op = render_state_transformer_get_blend_op(flags);
    VkColorComponentFlags color_mask = render_state_transformer_get_color_mask(flags);

    return (VkPipelineColorBlendAttachmentState){
        .blendEnable = src_factor != VK_BLEND_FACTOR_ONE || dst_factor != VK_BLEND_FACTOR_ZERO,
        .srcColorBlendFactor = src_factor,
        .dstColorBlendFactor = dst_factor,
        .colorBlendOp = blend_op,
        .srcAlphaBlendFactor = src_factor,
        .alphaBlendOp = blend_op,
        .colorWriteMask = color_mask,
    };
}

VkPipelineDepthStencilStateCreateInfo render_state_transformer_get_depth_stencil_state(
    RenderStateFlags flags, const Device* device) {
    VkCompareOp depth_compare_op = render_state_transformer_get_depth_compare_op(flags);

    VkStencilOpState front_stencil = render_state_transformer_get_front_stencil(flags);
    VkStencilOpState back_stencil = render_state_transformer_get_back_stencil(flags);

    bool gpu_depth_bounds = device != NULL ? device->physical_device->features.features.depthBounds : false;

    return (VkPipelineDepthStencilStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = (flags & RST_DEPTHMASK) == 0,
        .depthCompareOp = depth_compare_op,
        .depthBoundsTestEnable = gpu_depth_bounds && (flags & RST_DEPTHMASK) == 0,
        .stencilTestEnable = (flags & (RST_STENCIL_FUNC_BITS | RST_STENCIL_OP_BITS)) != 0,
        .front = front_stencil,
        .back = back_stencil,
        .minDepthBounds = gpu_depth_bounds ? 0.0f : 0,
        .maxDepthBounds = gpu_depth_bounds ? 1.0f : 0,
    };
}

VkPipelineMultisampleStateCreateInfo render_state_transformer_get_multisample_state(RenderStateFlags flags) {
    return (VkPipelineMultisampleStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0,
        .pSampleMask = NULL,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };
}

void render_state_transformer_get_dynamic_states(
    RenderStateFlags flags, const Device* device, VkDynamicState* dynamic_states, size_t* dynamic_states_size) {
    size_t i = 2;

    if (dynamic_states) {
        dynamic_states[0] = VK_DYNAMIC_STATE_SCISSOR;
        dynamic_states[1] = VK_DYNAMIC_STATE_VIEWPORT;
    }
    if (flags & RST_POLYGON_OFFSET) {
        if (dynamic_states) {
            dynamic_states[i] = VK_DYNAMIC_STATE_DEPTH_BIAS;
        }
        ++i;
    }
    bool gpu_depth_bounds = device != NULL ? device->physical_device->features.features.depthBounds : false;
    if (gpu_depth_bounds && (flags & RST_DEPTH_TEST_MASK)) {
        if (dynamic_states) {
            dynamic_states[i] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
        }
        ++i;
    }

    *dynamic_states_size = i;
}
