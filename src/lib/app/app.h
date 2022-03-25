#ifndef APP_H
#define APP_H

#include <stdbool.h>

#include "../vulkan/core/state.h"
#include "../window/app_window.h"

typedef struct App {
    AppWindow window;
    VulkanState state;
} App;

static inline App app_create() {
    App app = {.window = app_window_create(), .state = vulkan_state_create()};
    return app;
}

void app_init(App* app);
bool app_is_init(const App* app);
void app_start(App* app);
void app_destroy(App* app);

#endif
