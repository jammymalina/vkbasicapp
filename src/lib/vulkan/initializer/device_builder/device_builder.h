#ifndef DEVICE_BUILDER_H
#define DEVICE_BUILDER_H

#include <stdbool.h>
#include <stdint.h>

#include "../../core/device/device.h"
#include "../../core/errors.h"
#include "../../core/instance.h"
#include "../../core/physical_device/physical_device.h"
#include "../../core/queue/queue.h"

typedef struct DeviceBuilder {
    const PhysicalDevice* physical_device;

    VkDeviceCreateFlags flags;

    QueueDescriptor custom_queue_descriptors[DEVICE_MAX_QUEUE_DESCRIPTORS];
    uint32_t custom_queue_descriptor_count;
} DeviceBuilder;

static inline void device_builder_clear(DeviceBuilder* builder) {
    builder->physical_device = NULL;
    builder->flags = (VkDeviceCreateFlags){0};
    builder->custom_queue_descriptor_count = 0;
}

bool device_builder_add_custom_queue_descriptor(DeviceBuilder* builder, const QueueDescriptor* descriptor);
DeviceError device_builder_build(DeviceBuilder* builder, Device* device);

#endif
