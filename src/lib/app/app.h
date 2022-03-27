#ifndef APP_H
#define APP_H

#include <stdbool.h>

#include "../vulkan/core/state.h"
#include "../window/app_window.h"

typedef struct App {
    AppWindow window;
    VulkanState state;
} App;

static inline void app_clear(App* app) {
    app_window_clear(&app->window);
    vulkan_state_clear(&app->state);
}

void app_init(App* app);
bool app_is_init(const App* app);
void app_start(App* app);
void app_destroy(App* app);

#endif
