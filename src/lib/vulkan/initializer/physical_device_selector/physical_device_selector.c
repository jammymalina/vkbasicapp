#include "./physical_device_selector.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../../core/memory/memory.h"
#include "../../../core/string/string.h"
#include "../../core/functions.h"
#include "../../utils/queue.h"

static bool physical_device_add_extension(PhysicalDevice* device, const char* extension_name) {
    if (device->extension_count >= PHYSICAL_DEVICE_MAX_EXTENSIONS) {
        return false;
    }
    string_copy(extension_name, device->extensions[device->extension_count], VK_MAX_EXTENSION_NAME_SIZE);
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

static void physical_device_load_device_basic_props(PhysicalDevice* device) {
    vkGetPhysicalDeviceProperties(device->handle, &device->properties);
    vkGetPhysicalDeviceFeatures(device->handle, &device->features);
    vkGetPhysicalDeviceMemoryProperties(device->handle, &device->memory_properties);
    device->name = device->properties.deviceName;
}

static PhysicalDeviceError physical_device_load_queue_families(PhysicalDevice* device) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->handle, &count, NULL);
    if (count > PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES) {
        return QUEUE_FAMILY_COUNT_TOO_LARGE;
    }
    if (count == 0) {
        return NO_AVAILABLE_QUEUES;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(device->handle, &count, device->queue_families);
    device->queue_family_count = count;
    return PHYSICAL_DEVICE_NO_ERROR;
}

static PhysicalDeviceError physical_device_load_extensions(PhysicalDevice* device) {
    uint32_t extension_count = 0;
    VkResult status = vkEnumerateDeviceExtensionProperties(device->handle, NULL, &extension_count, NULL);
    ASSERT_VULKAN_STATUS(
        status, "Unable to enumerate physical device extensions", FAILED_ENUMERATE_PHYSICAL_DEVICE_EXTENSIONS);
    if (extension_count > 0) {
        VkExtensionProperties extensions[extension_count];
        status = vkEnumerateDeviceExtensionProperties(device->handle, NULL, &extension_count, extensions);
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

static void physical_device_selector_load_extended_feature_chain(
    PhysicalDeviceSelector* selector, PhysicalDevice* device) {
    if (device->properties.apiVersion < selector->instance->api_version ||
        selector->extended_features_chain.length == 0) {
        return;
    }
    physical_device_feature_items_copy(&selector->extended_features_chain, &device->extended_features_chain, true);

    device->features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    device->features2.pNext = physical_device_feature_items_get_head(&device->extended_features_chain);
    vkGetPhysicalDeviceFeatures2(device->handle, &device->features2);
}

static PhysicalDeviceError physical_device_selector_load_device_data(
    PhysicalDeviceSelector* selector, PhysicalDevice* device, VkPhysicalDevice handle) {
    PhysicalDeviceError status;
    device->handle = handle;

    physical_device_load_device_basic_props(device);
    status = physical_device_load_queue_families(device);
    ASSERT_NO_ERROR(status, status);
    status = physical_device_load_extensions(device);
    ASSERT_NO_ERROR(status, status);
    physical_device_selector_load_extended_feature_chain(selector, device);

    return PHYSICAL_DEVICE_NO_ERROR;
}

static bool physical_device_selector_validate_features(
    const VkPhysicalDeviceFeatures* required_features, const VkPhysicalDeviceFeatures* features) {
    if (required_features == NULL) {
        return true;
    }
    if (features == NULL) {
        return false;
    }

    byte* a = (byte*)required_features;
    byte* b = (byte*)features;

    VkBool32 feature_a, feature_b;
    for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures); i += sizeof(VkBool32)) {
        mem_copy(a + i, &feature_a, sizeof(VkBool32));
        mem_copy(b + i, &feature_b, sizeof(VkBool32));
        if (feature_a && !feature_b) {
            return false;
        }
    }

    return true;
}

static bool physical_device_selector_validate_extensions(
    const char* required_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS], uint32_t required_extension_count,
    const PhysicalDevice* device) {
    uint32_t match_count = 0;
    for (uint32_t i = 0; i < required_extension_count; i++) {
        for (uint32_t j = 0; j < device->extension_count; j++) {
            if (string_equal(required_extensions[i], device->extensions[j])) {
                match_count++;
            }
        }
    }
    return match_count == required_extension_count;
}

static Rating physical_device_selector_rate_device(PhysicalDeviceSelector* selector, const PhysicalDevice* device) {
    Rating rate = HIGH_RATING;

    // Check device name
    if (!string_is_empty(selector->device_name) && !string_equal(selector->device_name, device->name)) {
        return LOW_RATING;
    }

    // Check api version
    if (selector->instance->api_version > device->properties.apiVersion) {
        return LOW_RATING;
    }
    if (selector->desired_api_version > device->properties.apiVersion) {
        rate = MEDIUM_RATING;
    }

    // Check device type
    if (device->properties.deviceType != preferred_device_type_to_vulkan_type(selector->prefered_device_type)) {
        if (!selector->allow_any_type) {
            return LOW_RATING;
        }
        rate = MEDIUM_RATING;
    }

    // Check queues
    bool dedicated_compute = queue_utils_get_dedicated_queue_index(device->queue_families, device->queue_family_count,
                                 VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT) != UINT32_MAX;
    bool dedicated_transfer = queue_utils_get_dedicated_queue_index(device->queue_families, device->queue_family_count,
                                  VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) != UINT32_MAX;
    bool separate_compute = queue_utils_get_separate_queue_index(device->queue_families, device->queue_family_count,
                                VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT) != UINT32_MAX;
    bool separate_transfer = queue_utils_get_separate_queue_index(device->queue_families, device->queue_family_count,
                                 VK_QUEUE_TRANSFER_BIT, VK_QUEUE_COMPUTE_BIT) != UINT32_MAX;
    bool present_queue = queue_utils_get_present_queue_index(device, selector->instance) != UINT32_MAX;

    if (selector->require_dedicated_compute_queue && !dedicated_compute) return LOW_RATING;
    if (selector->require_dedicated_transfer_queue && !dedicated_transfer) return LOW_RATING;
    if (selector->require_separate_compute_queue && !separate_compute) return LOW_RATING;
    if (selector->require_separate_transfer_queue && !separate_transfer) return LOW_RATING;
    if (selector->require_present && !present_queue) return LOW_RATING;

    // Check present
    if (selector->require_present) {
        uint32_t formats_count = 0;
        uint32_t present_modes_count = 0;
        VkResult formats_res =
            vkGetPhysicalDeviceSurfaceFormatsKHR(device->handle, selector->instance->surface, &formats_count, NULL);
        VkResult present_modes_ret = vkGetPhysicalDeviceSurfacePresentModesKHR(
            device->handle, selector->instance->surface, &present_modes_count, NULL);

        if (formats_res != VK_SUCCESS || present_modes_ret != VK_SUCCESS || formats_count == 0 ||
            present_modes_count == 0) {
            return LOW_RATING;
        }
    }

    // Check extensions
    if (!physical_device_selector_validate_extensions(
            selector->required_extensions, selector->required_extension_count, device)) {
        return LOW_RATING;
    }
    if (!physical_device_selector_validate_extensions(
            selector->desired_extensions, selector->desired_extension_count, device)) {
        rate = MEDIUM_RATING;
    }

    // Check features
    if (selector->enabled_experimental_feature_validation) {
        if (!physical_device_selector_validate_features(&selector->required_features, &device->features)) {
            return LOW_RATING;
        }
        if (!physical_device_selector_validate_features(&selector->desired_features, &device->features)) {
            rate = MEDIUM_RATING;
        }

        if (!physical_device_feature_items_compare(
                &selector->extended_features_chain, &device->extended_features_chain)) {
            return LOW_RATING;
        }
    }

    // Check memory
    for (uint32_t i = 0; i < device->memory_properties.memoryHeapCount; i++) {
        if (device->memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            if (device->memory_properties.memoryHeaps[i].size < selector->required_mem_size) {
                return LOW_RATING;
            } else if (device->memory_properties.memoryHeaps[i].size < selector->desired_mem_size) {
                rate = MEDIUM_RATING;
            }
        }
    }

    return rate;
}

static void physical_device_selector_destroy_devices(
    const PhysicalDeviceSelector* selector, PhysicalDevice* devices, uint32_t device_count) {
    for (uint32_t i = 0; i < device_count; i++) {
        physical_device_destroy(&devices[i]);
    }
}

static void physical_device_selector_finalize_device(
    PhysicalDeviceSelector* selector, const PhysicalDevice* src, PhysicalDevice* dst) {
    dst->rating = src->rating;

    dst->features = selector->required_features;

    physical_device_feature_items_copy(&selector->extended_features_chain, &dst->extended_features_chain, false);
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

    if (selector->use_first_gpu_unconditionally) {
        status = physical_device_selector_load_device_data(selector, device, device_handles[0]);
        return status;
    }

    PhysicalDevice devices[device_count];
    for (uint32_t i = 0; i < device_count; i++) {
        physical_device_clear(&devices[i]);
    }

    status = PHYSICAL_DEVICE_NO_ERROR;
    for (uint32_t i = 0; i < device_count && status == PHYSICAL_DEVICE_NO_ERROR; i++) {
        status = physical_device_selector_load_device_data(selector, &devices[i], device_handles[i]);
    }
    ASSERT_NO_ERROR(status, status);

    for (uint32_t i = 0; i < device_count; i++) {
        Rating rating = physical_device_selector_rate_device(selector, &devices[i]);
        devices[i].rating = rating;
    }

    Rating max_rating = RATING_MAX_ENUM;
    PhysicalDevice* best_device = NULL;
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i].rating > max_rating) {
            best_device = &devices[i];
            max_rating = devices[i].rating;
        }
    }

    if (best_device == NULL || max_rating < MEDIUM_RATING) {
        physical_device_selector_destroy_devices(selector, devices, device_count);
        return NO_SUITABLE_DEVICE;
    }

    physical_device_selector_finalize_device(selector, best_device, device);
    physical_device_selector_destroy_devices(selector, devices, device_count);

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

bool physical_device_selector_add_extended_required_features_with_offset(
    PhysicalDeviceSelector* selector, void* features, size_t features_byte_size, size_t features_next_byte_offset) {
    return physical_device_feature_items_add(
        &selector->extended_features_chain, features, features_byte_size, features_next_byte_offset);
}

void physical_device_selector_destroy(PhysicalDeviceSelector* selector) {
    physical_device_feature_items_destroy(&selector->extended_features_chain);
    physical_device_selector_clear(selector);
}
