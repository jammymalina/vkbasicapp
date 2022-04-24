#ifndef APP_H
#define APP_H

#include <stdbool.h>

#include "../core/fs/path.h"
#include "../renderer/renderer.h"
#include "../vulkan/core/command/command_context.h"
#include "../vulkan/core/context/context.h"
#include "../vulkan/core/memory/memory_context.h"
#include "../vulkan/core/rendering/rendering_context.h"
#include "../vulkan/core/shader/pipeline_repository.h"
#include "./window/app_window.h"

typedef struct App {
    char basepath[PATH_MAX_SIZE];
    AppWindow window;
    Context context;
    PipelineRepository pipeline_repository;
    CommandContext command_context;
    MemoryContext memory_context;
    RenderingContext rendering_context;
    Renderer renderer;
    bool is_init;
} App;

static inline void app_clear(App* app) {
    string_copy("", app->basepath, PATH_MAX_SIZE);
    app_window_clear(&app->window);
    context_clear(&app->context);
    pipeline_repository_clear(&app->pipeline_repository);
    command_context_clear(&app->command_context);
    memory_context_clear(&app->memory_context);
    rendering_context_clear(&app->rendering_context);
    renderer_clear(&app->renderer);
    app->is_init = false;
}

void app_init(App* app);
bool app_is_init(const App* app);
int app_start(App* app);
void app_destroy(App* app);

#endif
