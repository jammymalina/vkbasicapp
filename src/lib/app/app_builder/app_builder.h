#ifndef APP_BUILDER_H
#define APP_BUILDER_H

#include <stdbool.h>

#include "../../vulkan/initializer/context_builder/context_builder.h"
#include "../app.h"
#include "../window/app_window_builder/app_window_builder.h"

typedef struct AppBuilder {
    AppWindowBuilder window_builder;
    ContextBuilder context_builder;
} AppBuilder;

static inline void app_builder_clear(AppBuilder* builder) {
    app_window_builder_clear(&builder->window_builder);
    context_builder_clear(&builder->context_builder);
}

bool app_builder_build(AppBuilder* builder, const char* config_file, App* app);

#endif
