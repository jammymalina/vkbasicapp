#include "./graphics_pipeline.h"

void graphics_pipeline_clear(GraphicsPipeline* pipeline) {
    pipeline->handle = VK_NULL_HANDLE;
    pipeline->layout = VK_NULL_HANDLE;
}

void graphics_pipeline_copy(const GraphicsPipeline* src, GraphicsPipeline* dst) {
    dst->handle = src->handle;
    dst->layout = src->layout;
    dst->device = src->device;
}

bool graphics_pipeline_is_init(GraphicsPipeline* pipeline) {
    return pipeline->handle != VK_NULL_HANDLE && pipeline->layout != VK_NULL_HANDLE && pipeline->device != NULL;
}

void graphics_pipeline_destroy(GraphicsPipeline* pipeline) {
    if (!graphics_pipeline_is_init(pipeline)) {
        return;
    }
    vkDeviceWaitIdle(pipeline->device->handle);
    vkDestroyPipelineLayout(pipeline->device->handle, pipeline->layout, NULL);
    vkDestroyPipeline(pipeline->device->handle, pipeline->handle, NULL);
}
