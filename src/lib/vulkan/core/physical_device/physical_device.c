#include "./physical_device.h"

#include <vulkan/vulkan_core.h>

#include "../../../core/memory/memory.h"

void physical_device_feature_items_set_next_node(void* node, const void* next_node) {
    byte* byte_ptr = (byte*)node;
    if (next_node == NULL) {
        mem_set(byte_ptr, 0, sizeof(void*));
        return;
    }
    const void** next_node_ptr = &next_node;
    mem_copy(next_node_ptr, byte_ptr + sizeof(VkStructureType), sizeof(void*));
}

void physical_device_feature_items_copy(
    const PhysicalDeviceFeatureItems* src, PhysicalDeviceFeatureItems* dst, bool clear_features) {
    physical_device_feature_items_destroy(dst);

    for (uint32_t i = 0; i < src->length; i++) {
        physical_device_feature_items_add(dst, src->items[i].features, src->items[i].features_byte_size);
        if (clear_features) {
            byte* node = (byte*)src->items[i].features;
            size_t data_byte_delta = sizeof(VkStructureType) + sizeof(void*);
            mem_set(node + data_byte_delta, 0, src->items[i].features_byte_size - data_byte_delta);
        }
    }
}

bool physical_device_feature_items_add(PhysicalDeviceFeatureItems* items, void* features, size_t features_byte_size) {
    if (items->length >= PHYSICAL_DEVICE_MAX_EXTENDED_FEATURES) {
        return false;
    }

    log_info("Size %lu", features_byte_size);
    void* node = mem_alloc(features_byte_size);
    ASSERT_ALLOC(node, "Unable to allocate physical device feature item", false);
    mem_copy(features, node, features_byte_size);
    physical_device_feature_items_set_next_node(node, NULL);

    if (items->length == 0) {
        items->items[0].features = node;
        items->length = 1;
        return true;
    }

    physical_device_feature_items_set_next_node(items->items[items->length - 1].features, node);
    items->items[items->length].features = node;
    items->items[items->length].features_byte_size = features_byte_size;
    items->length += 1;

    return true;
}

bool physical_device_feature_items_compare(
    const PhysicalDeviceFeatureItems* required_features, const PhysicalDeviceFeatureItems* device_features) {
    if (required_features->length == 0) {
        return true;
    }
    if (required_features->length != device_features->length) {
        return false;
    }

    for (uint32_t i = 0; i < required_features->length; i++) {
        if (required_features->items[i].features_byte_size != device_features->items[i].features_byte_size) {
            return false;
        }

        const byte* a = (byte*)(&required_features->items[i].features);
        const byte* b = (byte*)(&device_features->items[i].features);

        VkStructureType struct_type_a;
        VkStructureType struct_type_b;
        mem_copy(a, &struct_type_a, sizeof(VkStructureType));
        mem_copy(b, &struct_type_b, sizeof(VkStructureType));
        if (struct_type_a != struct_type_b) {
            return false;
        }

        size_t data_byte_delta = sizeof(VkStructureType) + sizeof(void*);
        VkBool32 feature_a, feature_b;

        for (size_t i = data_byte_delta; i < required_features->items[i].features_byte_size; i += sizeof(VkBool32)) {
            mem_copy(a + i, &feature_a, sizeof(VkBool32));
            mem_copy(b + i, &feature_b, sizeof(VkBool32));
            if (feature_a && !feature_b) {
                return false;
            }
        }
    }

    return true;
}

void physical_device_feature_items_destroy(PhysicalDeviceFeatureItems* items) {
    for (uint32_t i = 0; i < items->length; i++) {
        mem_free(items->items[i].features);
    }
    physical_device_feature_items_clear(items);
}

void physical_device_destroy(PhysicalDevice* device) {
    physical_device_feature_items_destroy(&device->extended_features_chain);
    physical_device_clear(device);
}
