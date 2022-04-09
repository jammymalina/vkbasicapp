#include "./physical_device.h"

#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../core/memory/memory.h"
#include "../../../core/string/string.h"

static void physical_device_feature_items_set_next_node(void* node, const void* next_node, size_t next_node_offset) {
    byte* byte_ptr = (byte*)node;
    const void** next_node_ptr = &next_node;
    mem_copy(next_node_ptr, byte_ptr + next_node_offset, sizeof(void*));
}

void physical_device_feature_items_copy(
    const PhysicalDeviceFeatureItems* src, PhysicalDeviceFeatureItems* dst, bool clear_features) {
    physical_device_feature_items_destroy(dst);

    for (uint32_t i = 0; i < src->length; ++i) {
        physical_device_feature_items_add(
            dst, src->items[i].features, src->items[i].features_byte_size, src->items[i].features_next_byte_offset);
        if (clear_features) {
            byte* node = (byte*)dst->items[i].features;
            size_t data_byte_delta = dst->items[i].features_next_byte_offset + sizeof(void*);
            mem_set(node + data_byte_delta, 0, dst->items[i].features_byte_size - data_byte_delta);
        }
    }
}

PhysicalDeviceFeatureItem* physical_device_feature_items_get_by_structure_type(
    PhysicalDeviceFeatureItems* items, VkStructureType feature_type) {
    for (uint32_t i = 0; i < items->length; ++i) {
        VkStructureType item_type;
        mem_copy(items->items[i].features, &item_type, sizeof(VkStructureType));
        if (item_type == feature_type) {
            return &items->items[i];
        }
    }
    return NULL;
}

bool physical_device_feature_items_add(
    PhysicalDeviceFeatureItems* items, void* features, size_t features_byte_size, size_t features_next_byte_offset) {
    if (items->length >= PHYSICAL_DEVICE_MAX_EXTENDED_FEATURES || features_byte_size == 0) {
        return false;
    }

    void* node = mem_alloc(features_byte_size);
    ASSERT_ALLOC(node, "Unable to allocate physical device feature item", false);
    mem_copy(features, node, features_byte_size);
    physical_device_feature_items_set_next_node(node, NULL, features_next_byte_offset);

    items->items[items->length].features = node;
    items->items[items->length].features_byte_size = features_byte_size;
    items->items[items->length].features_next_byte_offset = features_next_byte_offset;

    if (items->length > 0) {
        physical_device_feature_items_set_next_node(
            items->items[items->length - 1].features, node, items->items[items->length - 1].features_next_byte_offset);
    }

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

    for (uint32_t i = 0; i < required_features->length; ++i) {
        if (required_features->items[i].features_byte_size != device_features->items[i].features_byte_size) {
            return false;
        }
        if (required_features->items[i].features_next_byte_offset !=
            device_features->items[i].features_next_byte_offset) {
            return false;
        }

        const byte* a = (byte*)(required_features->items[i].features);
        const byte* b = (byte*)(device_features->items[i].features);

        VkStructureType struct_type_a;
        VkStructureType struct_type_b;
        mem_copy(a, &struct_type_a, sizeof(VkStructureType));
        mem_copy(b, &struct_type_b, sizeof(VkStructureType));
        if (struct_type_a != struct_type_b) {
            return false;
        }

        const size_t data_byte_delta = required_features->items[i].features_next_byte_offset + sizeof(void*);
        VkBool32 feature_a, feature_b;

        for (size_t iter = data_byte_delta; iter <= required_features->items[i].features_byte_size - sizeof(VkBool32);
             iter += sizeof(VkBool32)) {
            mem_copy(a + iter, &feature_a, sizeof(VkBool32));
            mem_copy(b + iter, &feature_b, sizeof(VkBool32));
            if (feature_a && !feature_b) {
                return false;
            }
        }
    }

    return true;
}

void physical_device_feature_items_destroy(PhysicalDeviceFeatureItems* items) {
    for (uint32_t i = 0; i < items->length; ++i) {
        mem_free(items->items[i].features);
    }
    physical_device_feature_items_clear(items);
}

bool physical_device_add_extension(PhysicalDevice* device, const char* extension_name) {
    if (device->extension_count >= PHYSICAL_DEVICE_MAX_EXTENSIONS) {
        return false;
    }
    string_copy(extension_name, device->extensions[device->extension_count], VK_MAX_EXTENSION_NAME_SIZE);
    device->extension_count += 1;
    return true;
}

bool physical_device_has_extension(const PhysicalDevice* device, const char* extension_name) {
    for (uint32_t i = 0; i < device->extension_count; ++i) {
        if (string_equals(extension_name, device->extensions[i])) {
            return true;
        }
    }
    return false;
}

void physical_device_destroy(PhysicalDevice* device) {
    physical_device_feature_items_destroy(&device->extended_features_chain);
    physical_device_clear(device);
}
