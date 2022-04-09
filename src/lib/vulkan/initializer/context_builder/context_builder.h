#ifndef CONTEXT_BUILDER_H
#define CONTEXT_BUILDER_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../../core/context/context.h"
#include "../../core/errors.h"
#include "../../initializer/device_builder/device_builder.h"
#include "../../initializer/instance_builder/instance_builder.h"
#include "../../initializer/physical_device_selector/physical_device_selector.h"

typedef struct ContextBuilder {
    void* window_handle;
    InstanceBuilder instance_builder;
    PhysicalDeviceSelector device_selector;
    DeviceBuilder device_builder;
} ContextBuilder;

static inline void context_builder_clear(ContextBuilder* builder) {
    builder->window_handle = NULL;
    instance_builder_clear(&builder->instance_builder);
    physical_device_selector_clear(&builder->device_selector);
    device_builder_clear(&builder->device_builder);
}

ContextError context_builder_build(ContextBuilder* builder, Context* context);
int context_builder_set_config_value(ContextBuilder* builder, const char* section, const char* name, const char* value);

#endif
