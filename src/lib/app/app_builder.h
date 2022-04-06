#ifndef APP_BUILDER_H
#define APP_BUILDER_H

#include <stdbool.h>

#include "../window/app_window.h"
#include "./app.h"

typedef struct AppBuilder {
    AppWindowBuilder window_builder;
} AppBuilder;

void app_builder_clear(AppBuilder* builder);
bool app_builder_build(AppBuilder* builder, const char* config_file, App* app);

#endif
