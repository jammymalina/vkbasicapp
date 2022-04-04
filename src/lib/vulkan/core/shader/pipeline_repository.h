#ifndef PIPELINE_REPOSITORY_H
#define PIPELINE_REPOSITORY_H

#include <stdbool.h>
#include <stddef.h>

#include "../../../core/collections/hash_string_map.h"
#include "./graphics_pipeline.h"
#include "./shader_types.h"

typedef struct PipelineRecord {
    PipelineType type;
    union {
        GraphicsPipeline graphics_pipeline;
    };
} PipelineRecord;

typedef struct PipelineHashMap HASH_STRING_MAP(PipelineRecord) PipelineHashMap;

typedef struct PipelineRepositoryConfig {
    size_t reserved_size;
} PipelineRepositoryConfig;

typedef struct PipelineRepository {
    PipelineHashMap pipeline_map;
} PipelineRepository;

void pipeline_repository_clear(PipelineRepository* repository);
bool pipeline_repository_init(PipelineRepository* repository, const PipelineRepositoryConfig* config);

bool pipeline_repository_add_graphics_pipeline(
    PipelineRepository* repository, const char* name, const GraphicsPipeline* pipeline);
const GraphicsPipeline* const pipeline_repository_get_graphics_pipeline(
    const PipelineRepository* repository, const char* name);

void pipeline_repository_destroy(PipelineRepository* repository);

#endif
