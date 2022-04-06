#include "./app_builder.h"

#include "../core/parsers/ini_parser.h"

static int app_builder_window_handler(AppBuilder* builder, const char* name, const char* value) {
    if (string_equal(name, "title")) {
        string_copy(value, builder->window_builder.title, APP_WINDOW_MAX_TITLE_SIZE);
        return 1;
    }
    if (string_equal(name, "x")) {
        builder->window_builder.x = string_to_int(value);
        return 1;
    }
    if (string_equal(name, "y")) {
        builder->window_builder.y = string_to_int(value);
        return 1;
    }
    if (string_equal(name, "width")) {
        builder->window_builder.width = string_to_int(value);
        return 1;
    }
    if (string_equal(name, "height")) {
        builder->window_builder.height = string_to_int(value);
        return 1;
    }
    if (string_equal(name, "fullscreen")) {
        builder->window_builder.fullscreen = string_equal(value, "1");
        return 1;
    }
    if (string_equal(name, "resizable")) {
        builder->window_builder.resizable = string_equal(value, "1");
        return 1;
    }

    log_warning("INI parser: Unknown field %s", name);
    return 1;
}

static int app_builder_parser_handler(
    void* user, const char* section, const char* name, const char* value, int lineno) {
    AppBuilder* builder = (AppBuilder*)user;
    if (section == NULL || name == NULL || value == NULL) {
        return 1;
    }

    if (string_equal(section, "window")) {
        return app_builder_window_handler(builder, name, value);
    }

    return 1;
}

void app_builder_clear(AppBuilder* builder) { app_window_builder_clear(&builder->window_builder); }

bool app_builder_build(AppBuilder* builder, const char* config_file, App* app) {
    app_builder_clear(builder);

    ini_parse(config_file, app_builder_parser_handler, builder);

    app_window_builder_build(&builder->window_builder, &app->window);
    if (!app->window.is_init) {
        log_error("Unable to create the window");
        return false;
    }

    return true;
}
