#include "./app.h"

void app_init(App* app) {
    // TODO: Add ini configuration for the app
    AppWindowBuilder builder = {0};
    app_window_builder_clear(&builder);
    builder.width = 1280;
    builder.height = 720;
    builder.title = "Basic app";
    app_window_builder_build(&builder, &app->window);

    ContextError context_status = context_init(&app->context, app->window.handle);
    if (context_status != CONTEXT_NO_ERROR) {
        return;
    }
    RenderingContextError rendering_context_status = rendering_context_init(&app->rendering_context, &app->context);
    if (rendering_context_status != RENDERING_CONTEXT_NO_ERROR) {
        return;
    }

    app->is_init = true;
}

bool app_is_init(const App* app) { return app->is_init; }

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
    rendering_context_destroy(&app->rendering_context);
    context_destroy(&app->context);
    app_window_destroy(&app->window);
    app_clear(app);
}
