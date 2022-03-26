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
    if (selector->extended_features_chain_length == 0) {
        return;
    }

    if (device->properties.apiVersion < selector->instance->api_version) {
        return;
    }

    PhysicalDeviceFeatureNode* prev = NULL;
    for (uint32_t i = 0; i < selector->extended_features_chain_length; i++) {
        if (prev != NULL) {
            if (prev->struct_type == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES) {
                prev->features_11.pNext = (void*)&selector->extended_features_chain[i];
            }
            if (prev->struct_type == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES) {
                prev->features_12.pNext = (void*)&selector->extended_features_chain[i];
            }
            if (prev->struct_type == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES) {
                prev->features_13.pNext = (void*)&selector->extended_features_chain[i];
            }
            prev = &selector->extended_features_chain[i];
        }
    }

    if (prev == NULL) {
        return;
    }

    if (prev->struct_type == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES) {
        prev->features_11.pNext = NULL;
    }
    if (prev->struct_type == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES) {
        prev->features_12.pNext = NULL;
    }
    if (prev->struct_type == VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES) {
        prev->features_13.pNext = NULL;
    }

    device->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device->features2.pNext = &selector->extended_features_chain[0];
    vkGetPhysicalDeviceFeatures2(device->handle, &device->features2);

    for (uint32_t i = 0; i < selector->extended_features_chain_length; i++) {
        device->extended_features_chain[i] = selector->extended_features_chain[i];
    }

    return;
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
    *device = physical_device_create();

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

bool physical_device_selector_add_required_features_11(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan11Features features_11) {
    if (selector->extended_features_chain_length >= PHYSICAL_DEVICE_MAX_FEATURE_NODES) {
        return false;
    }
    PhysicalDeviceFeatureNode* node = &selector->extended_features_chain[selector->extended_features_chain_length];
    node->struct_type = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    node->features_11 = features_11;

    selector->extended_features_chain_length += 1;
    return true;
}

bool physical_device_selector_add_required_features_12(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan12Features features_12) {
    if (selector->extended_features_chain_length >= PHYSICAL_DEVICE_MAX_FEATURE_NODES) {
        return false;
    }
    PhysicalDeviceFeatureNode* node = &selector->extended_features_chain[selector->extended_features_chain_length];
    node->struct_type = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    node->features_12 = features_12;

    selector->extended_features_chain_length += 1;
    return true;
}

bool physical_device_selector_add_required_features_13(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan13Features features_13) {
    if (selector->extended_features_chain_length >= PHYSICAL_DEVICE_MAX_FEATURE_NODES) {
        return false;
    }
    PhysicalDeviceFeatureNode* node = &selector->extended_features_chain[selector->extended_features_chain_length];
    node->struct_type = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    node->features_13 = features_13;

    selector->extended_features_chain_length += 1;
    return true;
}
