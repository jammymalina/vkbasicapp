#ifndef DEVICE_H
#define DEVICE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../functions.h"
#include "../physical_device/physical_device.h"
#include "../queue/queue.h"

#define DEVICE_MAX_QUEUE_DESCRIPTORS 32

typedef struct Device {
    VkDevice handle;
    const PhysicalDevice* physical_device;
    bool loaded_device_functions;
    QueueDescriptor queue_descriptors[DEVICE_MAX_QUEUE_DESCRIPTORS];
    uint32_t queue_descriptor_count;
} Device;

void device_clear(Device* device);
bool device_is_init(const Device* device);

const QueueDescriptor* device_get_queue_descriptor(const Device* device, uint32_t family_index);
bool device_get_queues(
    const Device* device, Queue* queues, uint32_t family_index, uint32_t start_queue_index, uint32_t count);
Queue device_get_present_queue(const Device* device);

void device_destroy(Device* device);

#endif
