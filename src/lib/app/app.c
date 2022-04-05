#include "./app.h"

#include "../renderer/core/rendering_context_config.h"
#include "../vulkan/initializer/shader/graphics_pipeline_builder/graphics_pipeline_builder.h"

static bool init_shaders(App* app) {
    GraphicsPipelineBuilder builder;
    GraphicsPipelineBuilderConfig config = graphics_pipeline_builder_get_default_config();
    config.basepath = app->basepath;
    config.device = &app->context.device;

    graphics_pipeline_builder_clear(&builder);
    bool status = graphics_pipeline_builder_init(&builder, &config);
    if (!status) {
        return false;
    }

    const char* shader_files[2] = {"shaders/test/triangle.vert.svm", "shaders/test/triangle.frag.svm"};
    builder.color_attachment_count = 1;
    builder.color_attachments = &app->rendering_context.swapchain.image_format;
    builder.shader_file_count = 2;
    builder.shader_files = shader_files;
    builder.render_state_flags = RST_BASIC_3D;

    GraphicsPipeline pipeline;
    graphics_pipeline_builder_start(&builder);
    status = graphics_pipeline_builder_build(&builder, &pipeline);
    if (!status) {
        graphics_pipeline_builder_destroy(&builder);
        return false;
    }

    pipeline_repository_add_graphics_pipeline(&app->pipeline_repository, "test", &pipeline);

    graphics_pipeline_builder_destroy(&builder);

    return true;
}

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

    if (!init_shaders(app)) {
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
