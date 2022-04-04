#include "./graphics_pipeline.h"

void graphics_pipeline_clear(GraphicsPipeline* pipeline) { pipeline->handle = VK_NULL_HANDLE; }

void graphics_pipeline_copy(const GraphicsPipeline* src, GraphicsPipeline* dst) { dst->handle = src->handle; }

bool graphics_pipeline_is_init(GraphicsPipeline* pipeline) {
    return pipeline->handle != VK_NULL_HANDLE && pipeline->device != NULL;
}
