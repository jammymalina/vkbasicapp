#include "./app.h"

#include "../vulkan/initializer/initializer.h"

void app_init(App* app) {
    // TODO: Add ini configuration for the app
    AppWindowBuilder builder = {0};
    app_window_builder_clear(&builder);
    builder.width = 1280;
    builder.height = 720;
    builder.title = "Basic app";
    app_window_builder_build(&builder, &app->window);

    vulkan_state_init(&app->state, app->window.handle);
}

bool app_is_init(const App* app) { return app->window.is_init && app->state.is_init; }

int app_start(App* app) {
    if (!app_is_init(app)) {
        return 1;
    }

    bool is_running = true;
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

    return 0;
}

void app_destroy(App* app) {
    vulkan_state_destroy(&app->state);
    app_window_destroy(&app->window);
    app_clear(app);
}
