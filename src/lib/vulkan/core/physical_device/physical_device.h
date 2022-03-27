#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../../core/rating.h"
#include "../../../core/string/string.h"
#include "../functions.h"

#define PHYSICAL_DEVICE_MAX_EXTENSIONS 256
#define PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES 32
#define PHYSICAL_DEVICE_MAX_EXTENDED_FEATURES 8

typedef struct PhysicalDeviceFeatureItem {
    void* features;
    size_t features_byte_size;
    size_t features_next_byte_offset;
} PhysicalDeviceFeatureItem;

typedef struct PhysicalDeviceFeatureItems {
    PhysicalDeviceFeatureItem items[PHYSICAL_DEVICE_MAX_EXTENDED_FEATURES];
    uint32_t length;
} PhysicalDeviceFeatureItems;

static inline void physical_device_feature_items_clear(PhysicalDeviceFeatureItems* items) {
    items->length = 0;
    for (uint32_t i = 0; i < PHYSICAL_DEVICE_MAX_EXTENDED_FEATURES; i++) {
        items->items[i].features = NULL;
        items->items[i].features_byte_size = 0;
        items->items[i].features_next_byte_offset = 0;
    }
}

static inline void* physical_device_feature_items_get_head(PhysicalDeviceFeatureItems* items) {
    if (items->length == 0) {
        return NULL;
    }
    return items->items[0].features;
}

void physical_device_feature_items_copy(
    const PhysicalDeviceFeatureItems* src, PhysicalDeviceFeatureItems* dst, bool clear_features);

bool physical_device_feature_items_add(
    PhysicalDeviceFeatureItems* items, void* features, size_t features_byte_size, size_t features_next_byte_offset);
bool physical_device_feature_items_compare(
    const PhysicalDeviceFeatureItems* required_features, const PhysicalDeviceFeatureItems* device_features);
void physical_device_feature_items_destroy(PhysicalDeviceFeatureItems* items);

typedef struct PhysicalDevice {
    VkPhysicalDevice handle;

    char name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];

    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory_properties;
    PhysicalDeviceFeatureItems extended_features_chain;

    VkQueueFamilyProperties queue_families[PHYSICAL_DEVICE_MAX_QUEUE_FAMILIES];
    uint32_t queue_family_count;

    char extensions[PHYSICAL_DEVICE_MAX_EXTENSIONS][VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t extension_count;

    Rating rating;
} PhysicalDevice;

static inline void physical_device_clear(PhysicalDevice* device) {
    device->handle = VK_NULL_HANDLE;
    device->queue_family_count = 0;
    device->extension_count = 0;
    device->rating = LOW_RATING;
    device->features = (VkPhysicalDeviceFeatures){0};
    device->properties = (VkPhysicalDeviceProperties){0};
    device->memory_properties = (VkPhysicalDeviceMemoryProperties){0};
    string_copy("", device->name, VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
    physical_device_feature_items_clear(&device->extended_features_chain);
}

bool physical_device_add_extension(PhysicalDevice* device, const char* extension_name);
bool physical_device_has_extension(const PhysicalDevice* device, const char* extension_name);
void physical_device_destroy(PhysicalDevice* device);

#endif
