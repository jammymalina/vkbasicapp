#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../core/rating.h"
#include "./functions.h"

#define PHYSICAL_DEVICE_MAX_EXTENSIONS 64
#define PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES 32
#define PHYSICAL_DEVICE_MAX_FEATURE_NODES 8

typedef struct PhysicalDeviceFeatureNode {
    VkStructureType struct_type;
    union {
        VkPhysicalDeviceVulkan11Features features_11;
        VkPhysicalDeviceVulkan12Features features_12;
        VkPhysicalDeviceVulkan13Features features_13;
    };
} PhysicalDeviceFeatureNode;

typedef struct PhysicalDevice {
    VkPhysicalDevice handle;

    const char* name;

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory_properties;
    PhysicalDeviceFeatureNode extended_features_chain[PHYSICAL_DEVICE_MAX_FEATURE_NODES];
    uint32_t extended_features_chain_length;

    VkQueueFamilyProperties queue_families[PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES];
    uint32_t queue_family_count;

    const char* extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS];
    uint32_t extension_count;

    Rating rating;
} PhysicalDevice;

static inline PhysicalDevice physical_device_create() {
    PhysicalDevice device = {
        .handle = VK_NULL_HANDLE,
        .name = "",
        .features = {},
        .features2 = {},
        .properties = {},
        .memory_properties = {},
        .extended_features_chain = {},
        .extended_features_chain_length = 0,
        .queue_families = {},
        .queue_family_count = 0,
        .extensions = {},
        .extension_count = 0,
        .rating = LOW_RATING,
    };
    return device;
}

#endif
