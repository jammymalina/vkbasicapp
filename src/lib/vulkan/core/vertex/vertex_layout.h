#ifndef VERTEX_LAYOUT_H
#define VERTEX_LAYOUT_H

#include <stddef.h>
#include <vulkan/vulkan.h>

#define VERTEX_LAYOUT_MAX_BINDING_DESCRIPTORS 8
#define VERTEX_LAYOUT_ATTRIBUTE_BINDING_DESCRIPTORS 8

typedef enum VertexLayoutType {
    VERTEX_LAYOUT_UNKNOWN = -1,
    VERTEX_LAYOUT_NO_VERTICES,
    VERTEX_LAYOUT_POS_2,
    VERTEX_LAYOUT_POS_2_COL_4,
    VERTEX_LAYOUT_POS_UV_2,
    VERTEX_LAYOUT_POS_NOR_3,
    VERTEX_LAYOUT_POS_NOR,
    VERTEX_LAYOUT_POS_NOR_UV_3,
    VERTEX_LAYOUT_POS_NOR_UV,
    VERTEX_LAYOUTS_TOTAL
} VertexLayoutType;

typedef struct VertexLayout {
    VkPipelineVertexInputStateCreateInfo input_state;

    VkVertexInputBindingDescription binding_descriptions[VERTEX_LAYOUT_MAX_BINDING_DESCRIPTORS];
    size_t binding_description_count;

    VkVertexInputAttributeDescription attribute_descriptions[VERTEX_LAYOUT_ATTRIBUTE_BINDING_DESCRIPTORS];
    size_t attribute_description_count;
} VertexLayout;

const VertexLayout* vertex_layout_get_all();

#endif
