#ifndef PHYSICAL_DEVICE_SELECTOR_H
#define PHYSICAL_DEVICE_SELECTOR_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../../core/errors.h"
#include "../../core/instance.h"
#include "../../core/physical_device.h"

typedef enum PreferredDeviceType { OTHER_DEVICE, INTEGRATED_GPU, DISCRETE_GPU, VIRTUAL_GPU, CPU } PreferredDeviceType;

typedef struct PhysicalDeviceSelector {
    const Instance* instance;

    PreferredDeviceType prefered_device_type;
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
    VkPhysicalDeviceFeatures2 required_features2;
    PhysicalDeviceFeatureNode extended_features_chain[PHYSICAL_DEVICE_MAX_FEATURE_NODES];
    uint32_t extended_features_chain_length;

    const char* required_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t required_extension_count;
    const char* desired_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t desired_extension_count;

    bool use_first_gpu_unconditionally;
    bool enable_portability_subset;
} PhysicalDeviceSelector;

static inline PhysicalDeviceSelector physical_device_selector_create() {
    PhysicalDeviceSelector selector = {
        .instance = NULL,
        .prefered_device_type = DISCRETE_GPU,
        .allow_any_type = true,
        .desired_api_version = VK_VERSION_1_3,
        .require_present = true,
        .require_dedicated_transfer_queue = false,
        .require_dedicated_compute_queue = false,
        .require_separate_transfer_queue = false,
        .require_separate_compute_queue = false,
        .required_mem_size = 0,
        .desired_mem_size = 0,
        .required_features = {},
        .required_features2 = {},
        .extended_features_chain = {},
        .extended_features_chain_length = 0,
        .required_extensions = {},
        .required_extension_count = 0,
        .desired_extensions = {},
        .desired_extension_count = 0,
        .use_first_gpu_unconditionally = false,
        .enable_portability_subset = false,
    };
    return selector;
};

PhysicalDeviceError physical_device_selector_select(PhysicalDeviceSelector* selector, PhysicalDevice* device);
bool physical_device_selector_add_required_extension(PhysicalDeviceSelector* selector, const char* extension_name);
bool physical_device_selector_add_desired_extension(PhysicalDeviceSelector* selector, const char* extension_name);
bool physical_device_selector_add_required_features_11(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan11Features features_11);
bool physical_device_selector_add_required_features_12(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan12Features features_12);
bool physical_device_selector_add_required_features_13(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan13Features features_13);

#endif
