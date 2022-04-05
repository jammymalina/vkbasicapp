#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../device/device.h"

typedef struct GraphicsPipeline {
    VkPipeline handle;
    VkPipelineLayout layout;
    const Device* device;
} GraphicsPipeline;

void graphics_pipeline_clear(GraphicsPipeline* pipeline);
void graphics_pipeline_copy(const GraphicsPipeline* src, GraphicsPipeline* dst);
bool graphics_pipeline_is_init(GraphicsPipeline* pipeline);

void graphics_pipeline_destroy(GraphicsPipeline* pipeline);

#endif
