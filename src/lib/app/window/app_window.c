#include "./app_window.h"

#include <SDL2/SDL.h>

void app_window_destroy(AppWindow* window) {
    if (window->handle) {
        SDL_DestroyWindow(window->handle);
    }
    app_window_clear(window);
    SDL_Quit();
}
