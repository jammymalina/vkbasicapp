#ifndef APP_BUILDER_H
#define APP_BUILDER_H

#include <stdbool.h>

#include "../../renderer/core/rendering_context_config.h"
#include "../../vulkan/initializer/context_builder/context_builder.h"
#include "../../vulkan/initializer/memory_context_builder/memory_context_builder.h"
#include "../app.h"
#include "../window/app_window_builder/app_window_builder.h"

typedef struct AppBuilder {
    AppWindowBuilder window_builder;
    ContextBuilder context_builder;
    MemoryContextBuilder memory_context_builder;
    RenderingContextConfig rendering_context_config;
} AppBuilder;

static inline void app_builder_clear(AppBuilder* builder) {
    app_window_builder_clear(&builder->window_builder);
    context_builder_clear(&builder->context_builder);
    memory_context_builder_clear(&builder->memory_context_builder);
    builder->rendering_context_config = rendering_context_config_default();
}

bool app_builder_build(AppBuilder* builder, const char* config_file, App* app);

#endif
