#ifndef PHYSICAL_DEVICE_SELECTOR_H
#define PHYSICAL_DEVICE_SELECTOR_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../../core/errors.h"
#include "../../core/instance.h"
#include "../../core/physical_device/physical_device.h"

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
    PhysicalDeviceFeatureItems extended_features_chain;

    const char* required_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t required_extension_count;
    const char* desired_extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t desired_extension_count;

    bool use_first_gpu_unconditionally;
    bool enable_portability_subset;
} PhysicalDeviceSelector;

static inline void physical_device_selector_clear(PhysicalDeviceSelector* selector) {
    selector->instance = NULL;
    selector->prefered_device_type = DISCRETE_GPU;
    selector->allow_any_type = true;
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
};

PhysicalDeviceError physical_device_selector_select(PhysicalDeviceSelector* selector, PhysicalDevice* device);

bool physical_device_selector_add_required_extension(PhysicalDeviceSelector* selector, const char* extension_name);
bool physical_device_selector_add_desired_extension(PhysicalDeviceSelector* selector, const char* extension_name);

void physical_device_selector_add_extended_required_features_11(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan11Features features);
void physical_device_selector_add_extended_required_features_12(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan12Features features);
void physical_device_selector_add_extended_required_features_13(
    PhysicalDeviceSelector* selector, VkPhysicalDeviceVulkan13Features features);

#endif
