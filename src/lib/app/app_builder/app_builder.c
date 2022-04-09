#include "./app_builder.h"

#include "../../core/parsers/ini_parser.h"
#include "../../core/string/string.h"

static int app_builder_parser_handler(
    void* user, const char* section, const char* name, const char* value, int lineno) {
    AppBuilder* builder = (AppBuilder*)user;
    if (section == NULL || name == NULL || value == NULL) {
        return 1;
    }

    if (string_equals(section, "window")) {
        return app_window_builder_set_config_value(&builder->window_builder, name, value);
    }
    if (string_equals(section, "app")) {
        return context_builder_set_config_value(&builder->context_builder, section, name, value);
    }
    if (string_equals(section, "features")) {
        return context_builder_set_config_value(&builder->context_builder, section, name, value);
    }
    if (string_equals(section, "optional_features")) {
        return context_builder_set_config_value(&builder->context_builder, section, name, value);
    }
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

    return true;
}