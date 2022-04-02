#include "./device.h"

#include "../../utils/queue.h"
#include "../functions.h"

void device_clear(Device* device) {
    device->handle = VK_NULL_HANDLE;
    device->physical_device = NULL;
    device->loaded_device_functions = false;
    device->queue_descriptor_count = 0;

    for (uint32_t i = 0; i < DEVICE_MAX_QUEUE_DESCRIPTORS; ++i) {
        queue_descriptor_clear(&device->queue_descriptors[i]);
    }
}

bool device_is_init(const Device* device) {
    return device->loaded_device_functions && device->handle != VK_NULL_HANDLE;
}

const QueueDescriptor* device_get_queue_descriptor(const Device* device, uint32_t family_index) {
    for (uint32_t i = 0; i < device->queue_descriptor_count; ++i) {
        if (device->queue_descriptors[i].family_index == family_index) {
            return &device->queue_descriptors[i];
        }
    }
    return NULL;
}

bool device_get_queues(
    const Device* device, Queue* queues, uint32_t family_index, uint32_t start_queue_index, uint32_t count) {
    const QueueDescriptor* descriptor = device_get_queue_descriptor(device, family_index);
    if (descriptor == NULL) {
        return false;
    }
    if (count > descriptor->count) {
        return false;
    }

    Queue* iter = queues;
    for (uint32_t i = start_queue_index; i < start_queue_index + count; ++i) {
        VkQueue handle;
        queue_clear(iter);
        vkGetDeviceQueue(device->handle, family_index, i, &handle);
        *iter = (Queue){.family_index = family_index, .handle = handle, .queue_index = i};
        ++iter;
    }

    return true;
}

Queue device_get_present_queue(const Device* device) {
    Queue queue;

    uint32_t present_queue_index = queue_utils_get_present_queue_index(device->physical_device);
    bool status = device_get_queues(device, &queue, present_queue_index, 0, 1);
    if (status) {
        return queue;
    }

    queue_clear(&queue);
    return queue;
}

void device_destroy(Device* device) {
    if (!device_is_init(device)) {
        return;
    }
    vkDestroyDevice(device->handle, NULL);
    device_clear(device);
}
