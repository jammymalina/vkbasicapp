#ifndef APP_H
#define APP_H

#include <stdbool.h>

#include "../vulkan/core/context/context.h"
#include "../vulkan/core/rendering/rendering_context.h"
#include "../window/app_window.h"

typedef struct App {
    AppWindow window;
    Context context;
    RenderingContext rendering_context;
    bool is_init;
} App;

static inline void app_clear(App* app) {
    app_window_clear(&app->window);
    context_clear(&app->context);
    rendering_context_clear(&app->rendering_context);
    app->is_init = false;
}

void app_init(App* app);
bool app_is_init(const App* app);
int app_start(App* app);
void app_destroy(App* app);

#endif
