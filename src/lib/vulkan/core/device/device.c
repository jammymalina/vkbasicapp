#include "./device.h"

#include "../functions.h"

void device_destroy(Device* device) {
    if (device->handle == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyDevice(device->handle, NULL);
    device_clear(device);
}
