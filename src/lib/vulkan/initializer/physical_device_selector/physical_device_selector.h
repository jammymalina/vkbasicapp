#ifndef PHYSICAL_DEVICE_SELECTOR_H
#define PHYSICAL_DEVICE_SELECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../../core/errors.h"
#include "../../core/instance.h"
#include "../../core/physical_device/physical_device.h"

typedef enum PreferredDeviceType { OTHER_DEVICE, INTEGRATED_GPU, DISCRETE_GPU, VIRTUAL_GPU, CPU } PreferredDeviceType;

static inline VkPhysicalDeviceType preferred_device_type_to_vulkan_type(PreferredDeviceType type) {
    switch (type) {
        case OTHER_DEVICE:
            return VK_PHYSICAL_DEVICE_TYPE_OTHER;
        case INTEGRATED_GPU:
            return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        case DISCRETE_GPU:
            return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        case VIRTUAL_GPU:
            return VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
        case CPU:
            return VK_PHYSICAL_DEVICE_TYPE_CPU;
        default:
            return VK_PHYSICAL_DEVICE_TYPE_OTHER;
    }
}

static inline PreferredDeviceType preferred_device_type_from_string(const char* type) {
    if (string_equals(type, "other")) {
        return OTHER_DEVICE;
    }
    if (string_equals(type, "integrated")) {
        return INTEGRATED_GPU;
    }
    if (string_equals(type, "discrete")) {
        return DISCRETE_GPU;
    }
    if (string_equals(type, "virtual")) {
        return VIRTUAL_GPU;
    }
    if (string_equals(type, "cpu")) {
        return CPU;
    }

    return OTHER_DEVICE;
}

typedef struct PhysicalDeviceExtension {
    char name[VK_MAX_EXTENSION_NAME_SIZE];
    bool enabled;
} PhysicalDeviceExtension;

typedef struct PhysicalDeviceSelector {
    const Instance* instance;

    const char* device_name;
    PreferredDeviceType preferred_device_type;
    bool allow_any_type;

    uint32_t desired_api_version;
    bool require_present;
    bool require_dedicated_transfer_queue;
    bool require_dedicated_compute_queue;
    bool require_separate_transfer_queue;
    bool require_separate_compute_queue;

    VkDeviceSize required_mem_size;
    VkDeviceSize desired_mem_size;

    VkPhysicalDeviceFeatures required_features;
    VkPhysicalDeviceFeatures desired_features;
    PhysicalDeviceFeatureItems extended_features_chain;
    bool experimental_feature_validation_enabled;

    PhysicalDeviceExtension required_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t required_extension_count;
    PhysicalDeviceExtension desired_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t desired_extension_count;

    bool use_first_gpu_unconditionally;
    bool enable_portability_subset;
} PhysicalDeviceSelector;

static inline void physical_device_selector_clear(PhysicalDeviceSelector* selector) {
    selector->instance = NULL;
    selector->preferred_device_type = DISCRETE_GPU;
    selector->allow_any_type = true;
    selector->device_name = "";
    selector->desired_api_version = VK_VERSION_1_3;
    selector->require_present = true;
    selector->require_dedicated_transfer_queue = false;
    selector->require_dedicated_compute_queue = false;
    selector->require_separate_transfer_queue = false;
    selector->require_separate_compute_queue = false;
    selector->required_mem_size = 0;
    selector->desired_mem_size = 0;
    selector->required_extension_count = 0;
    selector->desired_extension_count = 0;
    selector->use_first_gpu_unconditionally = false;
    selector->enable_portability_subset = false;

    selector->required_features = (VkPhysicalDeviceFeatures){0};
    selector->desired_features = (VkPhysicalDeviceFeatures){0};
    selector->experimental_feature_validation_enabled = false;
    physical_device_feature_items_clear(&selector->extended_features_chain);
};

PhysicalDeviceError physical_device_selector_select(PhysicalDeviceSelector* selector, PhysicalDevice* device);

bool physical_device_selector_set_required_extension(
    PhysicalDeviceSelector* selector, const char* extension_name, bool enabled);
bool physical_device_selector_set_desired_extension(
    PhysicalDeviceSelector* selector, const char* extension_name, bool enabled);

bool physical_device_selector_add_required_extension(PhysicalDeviceSelector* selector, const char* extension_name);
bool physical_device_selector_remove_required_extension(PhysicalDeviceSelector* selector, const char* extension_name);
bool physical_device_selector_add_desired_extension(PhysicalDeviceSelector* selector, const char* extension_name);
bool physical_device_selector_remove_desired_extension(PhysicalDeviceSelector* selector, const char* extension_name);

PhysicalDeviceFeatureItem* physical_device_selector_get_extended_required_features_item(
    PhysicalDeviceSelector* selector, VkStructureType feature_type);
bool physical_device_selector_add_extended_required_features_with_offset(
    PhysicalDeviceSelector* selector, void* features, size_t features_byte_size, size_t features_next_byte_offset);

#define physical_device_selector_add_extended_required_features(selector, features)                                    \
    physical_device_selector_add_extended_required_features_with_offset(                                               \
        selector, &features, sizeof(__typeof__(features)), offsetof(__typeof__(features), pNext))

void physical_device_selector_destroy(PhysicalDeviceSelector* selector);

#endif
