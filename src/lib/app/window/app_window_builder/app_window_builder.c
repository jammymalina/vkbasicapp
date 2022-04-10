#include "./app_window_builder.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include "../../../core/logger/logger.h"
#include "../../../core/parsers/ini_parser.h"
#include "../../../core/utils/flags.h"
#include "../app_window.h"

void app_window_builder_build(AppWindowBuilder* builder, AppWindow* window) {
    app_window_clear(window);

    SDL_Init(SDL_INIT_VIDEO);

    int flags = SDL_WINDOW_VULKAN;
    int x = builder->x, y = builder->y;
    int w = builder->width, h = builder->height;
    if (builder->fullscreen) {
        if (x == SDL_WINDOWPOS_UNDEFINED && y == SDL_WINDOWPOS_UNDEFINED) {
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            w = 0, h = 0;
        } else {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
    }

    if (!FLAGS_CHECK_FLAG(flags, SDL_WINDOW_FULLSCREEN_DESKTOP)) {
        if (w <= 0) w = 600;
        if (h <= 0) h = 600;
    }

    if (builder->resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    window->handle = SDL_CreateWindow(builder->title, x, y, w, h, flags);
    if (window->handle == NULL) {
        log_error("Unable to create the window: %s", SDL_GetError());
        return;
    }

    window->is_init = true;
}

int app_window_builder_set_config_value(AppWindowBuilder* builder, const char* name, const char* value) {
    if (string_equals(name, "title")) {
        string_copy(value, builder->title, APP_WINDOW_MAX_TITLE_SIZE);
        return 1;
    }
    if (string_equals(name, "x")) {
        INI_PARSER_ASSERT_INT("window", name, value, true, 1);
        builder->x = string_to_int(value, int);
        return 1;
    }
    if (string_equals(name, "y")) {
        INI_PARSER_ASSERT_INT("window", name, value, true, 1);
        builder->y = string_to_int(value, int);
        return 1;
    }
    if (string_equals(name, "width")) {
        INI_PARSER_ASSERT_INT("window", name, value, false, 1);
        builder->width = string_to_int(value, int);
        return 1;
    }
    if (string_equals(name, "height")) {
        INI_PARSER_ASSERT_INT("window", name, value, false, 1);
        builder->height = string_to_int(value, int);
        return 1;
    }
    if (string_equals(name, "fullscreen")) {
        builder->fullscreen = string_equals(value, "1");
        return 1;
    }
    if (string_equals(name, "resizable")) {
        builder->resizable = string_equals(value, "1");
        return 1;
    }

    log_warning("App configuration: Unknown field %s", name);

    return 1;
}
