#include "./app.h"

#include "../vulkan/initializer/initializer.h"

void app_init(App* app) {
    // TODO: Add ini configuration for the app
    AppWindowBuilder builder = app_window_builder_create();
    builder.width = 1280;
    builder.height = 720;
    builder.title = "Basic app";
    app->window = app_window_builder_build(&builder);

    vulkan_state_init(&app->state, app->window.handle);
}

bool app_is_init(const App* app) { return app->window.is_init && app->state.is_init; }

void app_start(App* app) {
    bool is_running = app_is_init(app);

    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        is_running = false;
                    }
                    break;
            }
        }
    }
}

void app_destroy(App* app) {
    vulkan_state_destroy(&app->state);
    app_window_destroy(&app->window);
}