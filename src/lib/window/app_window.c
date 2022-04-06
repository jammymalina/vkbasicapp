#include "./app_window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include "../core/logger/logger.h"

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

    if (!(flags & SDL_WINDOW_FULLSCREEN_DESKTOP)) {
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

void app_window_destroy(AppWindow* window) {
    if (window->handle) {
        SDL_DestroyWindow(window->handle);
    }
    app_window_clear(window);
    SDL_Quit();
}
