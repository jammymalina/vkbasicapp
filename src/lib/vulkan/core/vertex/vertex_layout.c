#include "./vertex_layout.h"

#include <stdbool.h>

static void vertex_layout_clear(VertexLayout* layout) {
    layout->input_state = (VkPipelineVertexInputStateCreateInfo){
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = NULL,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = NULL,
    };
    layout->binding_description_count = 0;
    layout->attribute_description_count = 0;
}

const VertexLayout* vertex_layout_get_all() {
    static VertexLayout vertex_layouts[VERTEX_LAYOUTS_TOTAL];
    static bool are_layouts_init = false;
    if (are_layouts_init) {
        return vertex_layouts;
    }

    for (size_t i = 0; i < VERTEX_LAYOUTS_TOTAL; ++i) {
        vertex_layout_clear(&vertex_layouts[i]);
    }

    VertexLayout* layout;
    // VERTEX_LAYOUT_POS_2
    layout = &vertex_layouts[VERTEX_LAYOUT_POS_2];

    layout->binding_description_count = 1;
    layout->binding_descriptions[0].binding = 0;
    layout->binding_descriptions[0].stride = 2 * sizeof(float);
    layout->binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    layout->attribute_description_count = 1;
    // position
    layout->attribute_descriptions[0].location = 0;
    layout->attribute_descriptions[0].binding = layout->binding_descriptions[0].binding;
    layout->attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    layout->attribute_descriptions[0].offset = 0;

    // VERTEX_LAYOUT_POS_2_COL_4
    layout = &vertex_layouts[VERTEX_LAYOUT_POS_2_COL_4];

    layout->binding_description_count = 1;
    layout->binding_descriptions[0].binding = 0;
    layout->binding_descriptions[0].stride = 2 * sizeof(float) + 4 * sizeof(float);
    layout->binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    layout->attribute_description_count = 2;
    // position
    layout->attribute_descriptions[0].location = 0;
    layout->attribute_descriptions[0].binding = layout->binding_descriptions[0].binding;
    layout->attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    layout->attribute_descriptions[0].offset = 0;
    // color
    layout->attribute_descriptions[1].location = 1;
    layout->attribute_descriptions[1].binding = layout->binding_descriptions[0].binding;
    layout->attribute_descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    layout->attribute_descriptions[1].offset = 2 * sizeof(float);

    // VERTEX_LAYOUT_POS_UV_2
    layout = &vertex_layouts[VERTEX_LAYOUT_POS_UV_2];

    layout->binding_description_count = 1;
    layout->binding_descriptions[0].binding = 0;
    layout->binding_descriptions[0].stride = 2 * sizeof(float) + 2 * sizeof(float);
    layout->binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    layout->attribute_description_count = 2;
    // position
    layout->attribute_descriptions[0].location = 0;
    layout->attribute_descriptions[0].binding = layout->binding_descriptions[0].binding;
    layout->attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    layout->attribute_descriptions[0].offset = 0;
    // color
    layout->attribute_descriptions[1].location = 1;
    layout->attribute_descriptions[1].binding = layout->binding_descriptions[0].binding;
    layout->attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    layout->attribute_descriptions[1].offset = 2 * sizeof(float);

    // VERTEX_LAYOUT_POS_NOR_3
    // VERTEX_LAYOUT_POS_NOR
    // VERTEX_LAYOUT_POS_NOR_UV_3
    // VERTEX_LAYOUT_POS_NOR_UV

    are_layouts_init = true;
    return vertex_layouts;
}
