#include "./app.h"

#include "../renderer/core/rendering_context_config.h"

void app_init(App* app) {
    // TODO: Add ini configuration for the app
    AppWindowBuilder builder = {0};
    app_window_builder_clear(&builder);
    builder.width = 1920;
    builder.height = 1080;
    builder.title = "Basic app";
    app_window_builder_build(&builder, &app->window);
    if (!path_get_basepath(app->basepath)) {
        log_error("Unable to get basepath");
        return;
    }
    log_info("Application basepath %s", app->basepath);

    ContextError context_status = context_init(&app->context, app->window.handle);
    ASSERT_NO_ERROR_LOG(context_status, ContextError, context_error_to_string);

    PipelineRepositoryConfig pipeline_repo_config = {
        .reserved_size = 100,
    };
    bool status = pipeline_repository_init(&app->pipeline_repository, &pipeline_repo_config);
    if (!status) {
        log_error("Unable to initialize pipeline repository");
        return;
    }

    command_context_init(&app->command_context, &app->context);

    RenderingContextConfig rendering_context_config = rendering_context_config_default();
    RenderingContextError render_ctx_status = rendering_context_init(
        &app->rendering_context, &app->command_context, &app->pipeline_repository, rendering_context_config);
    ASSERT_NO_ERROR_LOG(render_ctx_status, RenderingContextError, rendering_context_error_to_string);

    renderer_init(&app->renderer, &app->rendering_context);

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
        bool resized = false;
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
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                        event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        resized = true;
                    }
                    break;
            }
        }

        if (resized) {
            is_running = is_running && renderer_resize(&app->renderer);
        }

        is_running = is_running && renderer_render(&app->renderer);
    }

    return 0;
}

void app_destroy(App* app) {
    pipeline_repository_destroy(&app->pipeline_repository);
    rendering_context_destroy(&app->rendering_context);
    command_context_destroy(&app->command_context);
    context_destroy(&app->context);
    app_window_destroy(&app->window);
    app_clear(app);
}
