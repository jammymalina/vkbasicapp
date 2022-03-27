#include "./physical_device.h"

void physical_device_feature_items_clear(PhysicalDeviceFeatureItems* items) {
    VkPhysicalDeviceVulkan11Features features_11 = {0};
    features_11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features_11.pNext = NULL;
    items->items[0].feature_type = PHYSICAL_DEVICE_VULKAN_11_FEATURE_TYPE;
    items->items[0].features_11 = features_11;

    VkPhysicalDeviceVulkan12Features features_12 = {0};
    features_12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features_12.pNext = NULL;
    items->items[1].feature_type = PHYSICAL_DEVICE_VULKAN_12_FEATURE_TYPE;
    items->items[1].features_12 = features_12;

    VkPhysicalDeviceVulkan13Features features_13 = {0};
    features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_13.pNext = NULL;
    items->items[2].feature_type = PHYSICAL_DEVICE_VULKAN_13_FEATURE_TYPE;
    items->items[2].features_13 = features_13;
}

void physical_device_feature_items_create_chain(PhysicalDeviceFeatureItems* items) {
    items->items[0].features_11.pNext = &items->items[1].features_12;
    items->items[1].features_12.pNext = &items->items[2].features_13;
}
