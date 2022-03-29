#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "../device/device.h"
#include "../instance.h"
#include "../library.h"
#include "../physical_device/physical_device.h"
#include "../system_info/system_info.h"

typedef struct Context {
    Library library;
    SystemInfo system;
    Instance instance;
    PhysicalDevice physical_device;
    Device device;
} Context;

static inline void context_clear(Context* context) {
    library_clear(&context->library);
    system_info_clear(&context->system);
    instance_clear(&context->instance);
    physical_device_destroy(&context->physical_device);
    device_clear(&context->device);
}

ContextError context_init(Context* context, void* window_handle);
void context_destroy(Context* context);

#endif
