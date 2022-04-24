#include "./device_builder.h"

#include <vulkan/vulkan.h>

#include "../../initializer/function_loader/function_loader.h"

static DeviceError device_builder_validate(const DeviceBuilder* builder) {
    if (builder->physical_device == NULL) {
        return NO_PHYSICAL_DEVICE_PROVIDED;
    }

    return DEVICE_SUCCESS;
}

bool device_builder_add_custom_queue_descriptor(DeviceBuilder* builder, const QueueDescriptor* descriptor) {
    if (builder->custom_queue_descriptor_count >= DEVICE_MAX_QUEUE_DESCRIPTORS) {
        return false;
    }
    queue_descriptor_copy(descriptor, &builder->custom_queue_descriptors[builder->custom_queue_descriptor_count]);
    builder->custom_queue_descriptor_count += 1;
    return true;
}

DeviceError device_builder_build(DeviceBuilder* builder, Device* device) {
    device_clear(device);

    DeviceError status = device_builder_validate(builder);
    ASSERT_SUCCESS(status, status);

    if (builder->custom_queue_descriptor_count == 0) {
        bool desc_status = true;
        for (uint32_t i = 0; i < builder->physical_device->queue_family_count && desc_status; ++i) {
            QueueDescriptor descriptor;
            queue_descriptor_create_1p(&descriptor, i, 1, 1.0);
            device_builder_add_custom_queue_descriptor(builder, &descriptor);
        }
        if (!desc_status) {
            return TOO_MANY_QUEUE_DESCRIPTORS_REQUESTED;
        }
    }

    VkDeviceQueueCreateInfo queue_create_infos[builder->custom_queue_descriptor_count];
    for (uint32_t i = 0; i < builder->custom_queue_descriptor_count; ++i) {
        queue_create_infos[i] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = builder->custom_queue_descriptors[i].family_index,
            .queueCount = builder->custom_queue_descriptors[i].count,
            .pQueuePriorities = builder->custom_queue_descriptors[i].priorities,
        };
    }

    uint32_t extension_count = builder->physical_device->extension_count;
    if (builder->physical_device->instance->surface != VK_NULL_HANDLE) {
        if (!physical_device_has_extension(builder->physical_device, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            extension_count += 1;
        }
    }
    const char* extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS + 1];
    for (uint32_t i = 0; i < extension_count; ++i) {
        extensions[i] = builder->physical_device->extensions[i];
    }
    if (!physical_device_has_extension(builder->physical_device, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
        extensions[extension_count - 1] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    }

    VkPhysicalDeviceFeatures2 local_features2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = physical_device_feature_items_get_head(&builder->physical_device->extended_features_chain),
        .features = builder->physical_device->features.features,
    };

    VkDeviceCreateInfo device_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &local_features2,
        .flags = builder->flags,
        .queueCreateInfoCount = builder->custom_queue_descriptor_count,
        .pQueueCreateInfos = queue_create_infos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = extension_count,
        .ppEnabledExtensionNames = extensions,
        .pEnabledFeatures = NULL,
    };

    VkDevice handle;
    VkResult device_status = vkCreateDevice(builder->physical_device->handle, &device_info, NULL, &handle);
    ASSERT_VK_LOG(device_status, "Unable to create a device", FAILED_CREATE_DEVICE);

    device->handle = handle;
    device->physical_device = builder->physical_device;

    FunctionLoaderError load_status = function_loader_load_device_level_functions(device->handle);
    if (load_status != FUNCTION_LOADER_SUCCESS) {
        return FAILED_TO_LOAD_DEVICE_FUNCTIONS;
    }
    device->loaded_device_functions = true;

    for (uint32_t i = 0; i < builder->custom_queue_descriptor_count; ++i) {
        queue_descriptor_copy(&builder->custom_queue_descriptors[i], &device->queue_descriptors[i]);
    }
    device->queue_descriptor_count = builder->custom_queue_descriptor_count;

    return DEVICE_SUCCESS;
}
