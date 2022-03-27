#include "./physical_device_selector.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../core/functions.h"

static bool physical_device_add_extension(PhysicalDevice* device, const char* extension_name) {
    if (device->extension_count >= PHYSICAL_DEVICE_MAX_EXTENSIONS) {
        return false;
    }
    device->extensions[device->extension_count] = extension_name;
    device->extension_count += 1;
    return true;
}

static PhysicalDeviceError physical_device_selector_validate(const PhysicalDeviceSelector* selector) {
    if (selector->instance == NULL) {
        return NO_INSTANCE_PROVIDED;
    }
    if (selector->require_present && selector->instance->surface == VK_NULL_HANDLE) {
        return NO_SURFACE_PROVIDED;
    }

    return PHYSICAL_DEVICE_NO_ERROR;
}

static inline void physical_device_load_device_basic_props(PhysicalDevice* device) {
    vkGetPhysicalDeviceProperties(device->handle, &device->properties);
    vkGetPhysicalDeviceFeatures(device->handle, &device->features);
    vkGetPhysicalDeviceMemoryProperties(device->handle, &device->memory_properties);
    device->name = device->properties.deviceName;
}

static inline PhysicalDeviceError physical_device_load_queue_families(PhysicalDevice* device) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->handle, &count, NULL);
    if (count > PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES) {
        return QUEUE_FAMILY_COUNT_TOO_LARGE;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(device->handle, &device->queue_family_count, device->queue_families);
    return PHYSICAL_DEVICE_NO_ERROR;
}

static inline PhysicalDeviceError physical_device_load_extensions(PhysicalDevice* device) {
    uint32_t extension_count = 0;
    VkResult status = vkEnumerateDeviceExtensionProperties(device->handle, NULL, &extension_count, NULL);
    ASSERT_VULKAN_STATUS(
        status, "Unable to enumerate physical device extensions", FAILED_ENUMERATE_PHYSICAL_DEVICE_EXTENSIONS);
    if (extension_count > 0) {
        VkExtensionProperties extensions[extension_count];
        status = vkEnumerateDeviceExtensionProperties(device->handle, NULL, &extension_count, NULL);
        ASSERT_VULKAN_STATUS(
            status, "Unable to enumerate physical device extensions", FAILED_ENUMERATE_PHYSICAL_DEVICE_EXTENSIONS);

        bool add_ext_status = true;
        for (uint32_t i = 0; i < extension_count && add_ext_status; i++) {
            add_ext_status = physical_device_add_extension(device, extensions[i].extensionName);
        }
        if (!add_ext_status) {
            return TOO_MANY_PHYSICAL_DEVICE_EXTENSIONS_REQUESTED;
        }
    }

    return PHYSICAL_DEVICE_NO_ERROR;
}

static inline void physical_device_selector_load_extended_feature_chain(
    PhysicalDeviceSelector* selector, PhysicalDevice* device) {
    if (device->properties.apiVersion < selector->instance->api_version) {
        return;
    }
    physical_device_feature_items_create_chain(&device->extended_features_chain);

    device->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device->features2.pNext = physical_device_feature_items_get_head(&device->extended_features_chain);
    vkGetPhysicalDeviceFeatures2(device->handle, &device->features2);
}

static PhysicalDeviceError physical_device_selector_load_device(
    PhysicalDeviceSelector* selector, PhysicalDevice* device, VkPhysicalDevice handle) {
    PhysicalDeviceError status;
    device->handle = handle;

    physical_device_load_device_basic_props(device);
    status = physical_device_load_queue_families(device);
    ASSERT_NO_ERROR(status, status);
    status = physical_device_load_queue_families(device);
    ASSERT_NO_ERROR(status, status);
    physical_device_selector_load_extended_feature_chain(selector, device);

    return PHYSICAL_DEVICE_NO_ERROR;
}

PhysicalDeviceError physical_device_selector_select(PhysicalDeviceSelector* selector, PhysicalDevice* device) {
    physical_device_clear(device);

    PhysicalDeviceError status = physical_device_selector_validate(selector);
    ASSERT_NO_ERROR(status, status);

    uint32_t device_count = 0;
    VkResult device_status = vkEnumeratePhysicalDevices(selector->instance->handle, &device_count, NULL);
    ASSERT_VULKAN_STATUS(device_status, "Unable to enumerate physical devices", FAILED_ENUMERATE_PHYSICAL_DEVICES);
    if (device_count == 0) {
        return NO_PHYSICAL_DEVICES_FOUND;
    }

    VkPhysicalDevice device_handles[device_count];
    device_status = vkEnumeratePhysicalDevices(selector->instance->handle, &device_count, device_handles);
    ASSERT_VULKAN_STATUS(device_status, "Unable to enumerate physical devices", FAILED_ENUMERATE_PHYSICAL_DEVICES);

    log_info("Found %d physical device(s)", device_count);

    if (selector->use_first_gpu_unconditionally || true) {
        status = physical_device_selector_load_device(selector, device, device_handles[0]);
        return status;
    }

    return PHYSICAL_DEVICE_NO_ERROR;
}

bool physical_device_selector_add_required_extension(PhysicalDeviceSelector* selector, const char* extension_name) {
    if (selector->required_extension_count >= PHYSICAL_DEVICE_MAX_EXTENSIONS) {
        return false;
    }
    selector->required_extensions[selector->required_extension_count] = extension_name;
    selector->required_extension_count += 1;
    return true;
}

bool physical_device_selector_add_desired_extension(PhysicalDeviceSelector* selector, const char* extension_name) {
    if (selector->desired_extension_count >= PHYSICAL_DEVICE_MAX_EXTENSIONS) {
        return false;
    }
    selector->desired_extensions[selector->desired_extension_count] = extension_name;
    selector->desired_extension_count += 1;
    return true;
}

void physical_device_selector_add_extended_required_features_11(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan11Features features) {
    selector->extended_features_chain.items[PHYSICAL_DEVICE_VULKAN_11_FEATURE_TYPE].features_11 = features;
}

void physical_device_selector_add_extended_required_features_12(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan12Features features) {
    selector->extended_features_chain.items[PHYSICAL_DEVICE_VULKAN_12_FEATURE_TYPE].features_12 = features;
}

void physical_device_selector_add_extended_required_features_13(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan13Features features) {
    selector->extended_features_chain.items[PHYSICAL_DEVICE_VULKAN_13_FEATURE_TYPE].features_13 = features;
}
