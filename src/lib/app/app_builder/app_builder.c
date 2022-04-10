#include "./app_builder.h"

#include <stdint.h>

#include "../../core/parsers/ini_parser.h"
#include "../../core/string/string.h"
#include "../../renderer/core/color/color_transformer.h"
#include "../../vulkan/core/errors.h"

static int app_builder_rendering_context_config_set_value(AppBuilder* builder, const char* name, const char* value) {
    if (string_equals(name, "frames_in_flight")) {
        INI_PARSER_ASSERT_INT("rendering_context", name, value, false, 1);
        builder->rendering_context_config.frames_in_flight = string_to_int(value, uint32_t);
        return 1;
    }
    if (string_equals(name, "clear_color")) {
        builder->rendering_context_config.clear_color = color_from_hex_string(value);
        return 1;
    }
    if (string_equals(name, "depth_enabled")) {
        builder->rendering_context_config.depth_enabled = string_equals(value, "1");
        return 1;
    }
    if (string_equals(name, "render_timeout_ms")) {
        INI_PARSER_ASSERT_INT("rendering_context", name, value, false, 1);
        builder->rendering_context_config.render_timeout_ms = string_to_int(value, uint64_t);
        return 1;
    }
    return 1;
}

static int app_builder_parser_handler(
    void* user, const char* section, const char* name, const char* value, int lineno) {
    AppBuilder* builder = (AppBuilder*)user;
    if (section == NULL || name == NULL || value == NULL) {
        return 1;
    }

    if (string_equals(section, "window")) {
        return app_window_builder_set_config_value(&builder->window_builder, name, value);
    }

    if (string_equals(section, "rendering_context")) {
        return app_builder_rendering_context_config_set_value(builder, name, value);
    }

    context_builder_set_config_value(&builder->context_builder, section, name, value);
    return 1;
}

bool app_builder_build(AppBuilder* builder, const char* config_file, App* app) {
    app_builder_clear(builder);

    ini_parse(config_file, app_builder_parser_handler, builder);

    app_window_builder_build(&builder->window_builder, &app->window);
    if (!app->window.is_init) {
        log_error("Unable to create the window");
        return false;
    }

    builder->context_builder.window_handle = app->window.handle;
    ContextError context_status = context_builder_build(&builder->context_builder, &app->context);

    ASSERT_SUCCESS_LOG(context_status, ContextError, context_error_to_string, false);

    command_context_init(&app->command_context, &app->context);

    RenderingContextError render_ctx_status = rendering_context_init(
        &app->rendering_context, &app->command_context, &app->pipeline_repository, builder->rendering_context_config);
    ASSERT_SUCCESS_LOG(render_ctx_status, RenderingContextError, rendering_context_error_to_string, false);

    renderer_init(&app->renderer, &app->rendering_context);

    return true;
}
