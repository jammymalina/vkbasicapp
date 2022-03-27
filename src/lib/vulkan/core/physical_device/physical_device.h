#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../../core/rating.h"
#include "../functions.h"

#define PHYSICAL_DEVICE_MAX_EXTENSIONS 64
#define PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES 32

typedef enum PhysicalDeviceFeatureType {
    PHYSICAL_DEVICE_VULKAN_11_FEATURE_TYPE,
    PHYSICAL_DEVICE_VULKAN_12_FEATURE_TYPE,
    PHYSICAL_DEVICE_VULKAN_13_FEATURE_TYPE,
    PHYSICAL_DEVICE_FEATURE_TYPE_COUNT,
} PhysicalDeviceFeatureType;

typedef struct PhysicalDeviceFeatureItem {
    PhysicalDeviceFeatureType feature_type;
    union {
        VkPhysicalDeviceVulkan11Features features_11;
        VkPhysicalDeviceVulkan12Features features_12;
        VkPhysicalDeviceVulkan13Features features_13;
    };
} PhysicalDeviceFeatureItem;

typedef struct PhysicalDeviceFeatureItems {
    PhysicalDeviceFeatureItem items[PHYSICAL_DEVICE_FEATURE_TYPE_COUNT];
} PhysicalDeviceFeatureItems;

void physical_device_feature_items_clear(PhysicalDeviceFeatureItems* items);
void physical_device_feature_items_create_chain(PhysicalDeviceFeatureItems* items);

static inline void* physical_device_feature_items_get_head(PhysicalDeviceFeatureItems* items) {
    return &items->items[0].features_11;
}

typedef struct PhysicalDevice {
    VkPhysicalDevice handle;

    const char* name;

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory_properties;
    PhysicalDeviceFeatureItems extended_features_chain;

    VkQueueFamilyProperties queue_families[PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES];
    uint32_t queue_family_count;

    const char* extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t extension_count;

    Rating rating;
} PhysicalDevice;

static inline void physical_device_clear(PhysicalDevice* device) {
    device->handle = VK_NULL_HANDLE;
    device->name = "";
    device->queue_family_count = 0;
    device->extension_count = 0;
    device->rating = LOW_RATING;
    physical_device_feature_items_clear(&device->extended_features_chain);
}

#endif
