#ifndef RENDER_STATE_TRANSFORMER_H
#define RENDER_STATE_TRANSFORMER_H

#include <vulkan/vulkan.h>

#include "../../../core/device/device.h"
#include "../../../core/rendering/render_state_bits.h"

#define RENDER_STATE_MAX_DYNAMIC_STATES 4

VkPipelineRasterizationStateCreateInfo render_state_transformer_get_rasterization_state(RenderStateFlags flags);
VkPipelineColorBlendAttachmentState render_state_transformer_get_color_blend_attachment(RenderStateFlags flags);
VkPipelineDepthStencilStateCreateInfo render_state_transformer_get_depth_stencil_state(
    RenderStateFlags flags, const Device* device);
VkPipelineMultisampleStateCreateInfo render_state_transformer_get_multisample_state(RenderStateFlags flags);
void render_state_transformer_get_dynamic_states(
    RenderStateFlags flags, const Device* device, VkDynamicState* dynamic_states, size_t* dynamic_states_size);

#endif
