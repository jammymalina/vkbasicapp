#ifndef APP_WINDOW_BUILDER_H
#define APP_WINDOW_BUILDER_H

#include <stdbool.h>

#include "../app_window.h"

typedef struct AppWindowBuilder {
    char title[APP_WINDOW_MAX_TITLE_SIZE];
    int x, y;
    int width, height;
    bool fullscreen;
    bool resizable;
} AppWindowBuilder;

static inline void app_window_builder_clear(AppWindowBuilder* builder) {
    string_copy("", builder->title, APP_WINDOW_MAX_TITLE_SIZE);
    builder->x = SDL_WINDOWPOS_UNDEFINED;
    builder->y = SDL_WINDOWPOS_UNDEFINED;
    builder->width = 0;
    builder->height = 0;
    builder->fullscreen = false;
    builder->resizable = false;
}

void app_window_builder_build(AppWindowBuilder* builder, AppWindow* app_window);
int app_window_builder_set_config_value(AppWindowBuilder* builder, const char* name, const char* value);

#endif
