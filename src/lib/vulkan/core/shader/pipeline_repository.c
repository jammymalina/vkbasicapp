#include "./pipeline_repository.h"

#include "graphics_pipeline.h"
#include "shader_types.h"

void pipeline_repository_clear(PipelineRepository* repository) { hash_string_map_clear(&repository->pipeline_map); }

bool pipeline_repository_init(PipelineRepository* repository, const PipelineRepositoryConfig* config) {
    size_t reserved_size = config->reserved_size < 10 ? 20 : config->reserved_size;
    bool status = hash_string_map_reserve(&repository->pipeline_map, reserved_size);

    return status;
}

bool pipeline_repository_add_graphics_pipeline(
    PipelineRepository* repository, const char* name, const GraphicsPipeline* pipeline) {
    if (string_length(name) >= HASH_KEY_MAX_SIZE) {
        return false;
    }

    bool add_status = hash_string_map_add(&repository->pipeline_map, name, (PipelineRecord){0});
    if (!add_status) {
        return false;
    }

    PipelineRecord* record = hash_string_map_get_reference(&repository->pipeline_map, name);
    if (record == NULL) {
        return false;
    }

    record->type = PIPELINE_TYPE_GRAPHICS;
    graphics_pipeline_copy(pipeline, &record->graphics_pipeline);

    return true;
}

const GraphicsPipeline* const pipeline_repository_get_graphics_pipeline(
    const PipelineRepository* repository, const char* name) {
    const PipelineRecord* record = hash_string_map_get_reference(&repository->pipeline_map, name);
    if (record == NULL || record->type != PIPELINE_TYPE_GRAPHICS) {
        return NULL;
    }
    return &record->graphics_pipeline;
}

void pipeline_repository_destroy(PipelineRepository* repository) {
    const size_t buffer_size = 32;
    size_t processed = 0;
    PipelineRecord* buffer[buffer_size];
    while (processed < hash_string_map_get_size(&repository->pipeline_map)) {
        const size_t current_processed =
            hash_string_map_values_reference_range(&repository->pipeline_map, buffer, processed, buffer_size);
        processed += current_processed;
        for (size_t i = 0; i < current_processed; ++i) {
            if (buffer[i]->type == PIPELINE_TYPE_GRAPHICS) {
                GraphicsPipeline* pipeline = &buffer[i]->graphics_pipeline;
                if (graphics_pipeline_is_init(pipeline)) {
                    vkDestroyPipeline(pipeline->device->handle, pipeline->handle, NULL);
                }
            }
        }
    }
    hash_string_map_clear(&repository->pipeline_map);
}
