#include "./context_builder.h"

#include <vulkan/vulkan_core.h>

#include "../../../core/memory/memory.h"
#include "../function_loader/function_loader.h"

typedef struct ContextBuilderFeatureItem {
    char feature_name[64];
    size_t feature_offset;
} ContextBuilderFeatureItem;

#define CONTEXT_BUILDER_FEATURE_TYPE_ITEM(type, name)                                                                  \
    { .feature_name = #name, .feature_offset = offsetof(type, name), }

#define CONTEXT_BUILDER_FEATURE_ITEM(name) CONTEXT_BUILDER_FEATURE_TYPE_ITEM(VkPhysicalDeviceFeatures, name)
#define CONTEXT_BUILDER_FEATURE_11_ITEM(name) CONTEXT_BUILDER_FEATURE_TYPE_ITEM(VkPhysicalDeviceVulkan11Features, name)
#define CONTEXT_BUILDER_FEATURE_12_ITEM(name) CONTEXT_BUILDER_FEATURE_TYPE_ITEM(VkPhysicalDeviceVulkan12Features, name)
#define CONTEXT_BUILDER_FEATURE_13_ITEM(name) CONTEXT_BUILDER_FEATURE_TYPE_ITEM(VkPhysicalDeviceVulkan13Features, name)

static const ContextBuilderFeatureItem* context_builder_get_feature_item(
    const char* name, const ContextBuilderFeatureItem* features, size_t feature_count) {
    for (size_t i = 0; i < feature_count; ++i) {
        if (string_equals(name, features[i].feature_name)) {
            return &features[i];
        }
    }
    return NULL;
}

static int context_builder_add_feature(ContextBuilder* builder, const char* name, const char* value, bool required) {
    VkBool32 feature_value = string_equals(value, "1") ? VK_TRUE : VK_FALSE;
    const ContextBuilderFeatureItem features[] = {
        CONTEXT_BUILDER_FEATURE_ITEM(robustBufferAccess),
        CONTEXT_BUILDER_FEATURE_ITEM(fullDrawIndexUint32),
        CONTEXT_BUILDER_FEATURE_ITEM(imageCubeArray),
        CONTEXT_BUILDER_FEATURE_ITEM(independentBlend),
        CONTEXT_BUILDER_FEATURE_ITEM(geometryShader),
        CONTEXT_BUILDER_FEATURE_ITEM(tessellationShader),
        CONTEXT_BUILDER_FEATURE_ITEM(sampleRateShading),
        CONTEXT_BUILDER_FEATURE_ITEM(dualSrcBlend),
        CONTEXT_BUILDER_FEATURE_ITEM(logicOp),
        CONTEXT_BUILDER_FEATURE_ITEM(multiDrawIndirect),
        CONTEXT_BUILDER_FEATURE_ITEM(drawIndirectFirstInstance),
        CONTEXT_BUILDER_FEATURE_ITEM(depthClamp),
        CONTEXT_BUILDER_FEATURE_ITEM(depthBiasClamp),
        CONTEXT_BUILDER_FEATURE_ITEM(fillModeNonSolid),
        CONTEXT_BUILDER_FEATURE_ITEM(depthBounds),
        CONTEXT_BUILDER_FEATURE_ITEM(wideLines),
        CONTEXT_BUILDER_FEATURE_ITEM(largePoints),
        CONTEXT_BUILDER_FEATURE_ITEM(alphaToOne),
        CONTEXT_BUILDER_FEATURE_ITEM(multiViewport),
        CONTEXT_BUILDER_FEATURE_ITEM(samplerAnisotropy),
        CONTEXT_BUILDER_FEATURE_ITEM(textureCompressionETC2),
        CONTEXT_BUILDER_FEATURE_ITEM(textureCompressionASTC_LDR),
        CONTEXT_BUILDER_FEATURE_ITEM(textureCompressionBC),
        CONTEXT_BUILDER_FEATURE_ITEM(occlusionQueryPrecise),
        CONTEXT_BUILDER_FEATURE_ITEM(pipelineStatisticsQuery),
        CONTEXT_BUILDER_FEATURE_ITEM(vertexPipelineStoresAndAtomics),
        CONTEXT_BUILDER_FEATURE_ITEM(fragmentStoresAndAtomics),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderTessellationAndGeometryPointSize),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderImageGatherExtended),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderStorageImageExtendedFormats),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderStorageImageMultisample),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderStorageImageReadWithoutFormat),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderStorageImageWriteWithoutFormat),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderUniformBufferArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderSampledImageArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderStorageBufferArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderStorageImageArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderClipDistance),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderCullDistance),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderFloat64),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderInt64),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderInt16),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderResourceResidency),
        CONTEXT_BUILDER_FEATURE_ITEM(shaderResourceMinLod),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseBinding),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidencyBuffer),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidencyImage2D),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidencyImage3D),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidency2Samples),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidency4Samples),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidency8Samples),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidency16Samples),
        CONTEXT_BUILDER_FEATURE_ITEM(sparseResidencyAliased),
        CONTEXT_BUILDER_FEATURE_ITEM(variableMultisampleRate),
        CONTEXT_BUILDER_FEATURE_ITEM(inheritedQueries),
    };
    const size_t feature_count = sizeof(features) / sizeof(ContextBuilderFeatureItem);
    const ContextBuilderFeatureItem* feature = context_builder_get_feature_item(name, features, feature_count);
    if (feature == NULL) {
        log_warning("Unknown device feature %s", name);
        return 1;
    }

    if (required) {
        mem_copy(&feature_value, ((byte*)&builder->device_selector.required_features) + feature->feature_offset,
            sizeof(VkBool32));
        return 1;
    }
    mem_copy(&feature_value, ((byte*)&builder->device_selector.desired_features) + feature->feature_offset,
        sizeof(VkBool32));

    return 1;
}

static int context_builder_add_feature_11(ContextBuilder* builder, const char* name, const char* value) {
    VkBool32 feature_value = string_equals(value, "1") ? VK_TRUE : VK_FALSE;
    const ContextBuilderFeatureItem features[] = {};
    const size_t feature_count = sizeof(features) / sizeof(ContextBuilderFeatureItem);
    const ContextBuilderFeatureItem* feature = context_builder_get_feature_item(name, features, feature_count);
    if (feature == NULL) {
        log_warning("Unknown device extended feature %s", name);
        return 1;
    }

    return 1;
}

ContextError context_builder_build(ContextBuilder* builder, Context* context) {
    ASSERT_SUCCESS_LOG(library_load(&context->library), LibraryError, library_error_to_string, CONTEXT_INIT_ERROR);
    function_loader_load_external_function((PFN_vkGetInstanceProcAddr)context->library.load_function);
    ASSERT_SUCCESS_LOG(function_loader_load_global_functions(), FunctionLoaderError, function_loader_error_to_string,
        CONTEXT_INIT_ERROR);

    if (!system_info_init(&context->system)) {
        return CONTEXT_INIT_ERROR;
    }

    builder->instance_builder.system = &context->system;
    builder->instance_builder.window_handle = builder->window_handle;
#ifdef DEBUG
    builder->instance_builder.debug_enabled = true;
    instance_builder_add_layer(&builder->instance_builder, "VK_LAYER_KHRONOS_validation");
#endif
    ASSERT_SUCCESS_LOG(instance_builder_build(&builder->instance_builder, &context->instance), InstanceError,
        instance_error_to_string, CONTEXT_INIT_ERROR);

    builder->device_selector.instance = &context->instance;

    return true;
}

int context_builder_set_config_value(
    ContextBuilder* builder, const char* section, const char* name, const char* value) {
    if (string_equals(section, "app")) {
        if (string_equals(name, "name")) {
            string_copy(value, builder->instance_builder.app_name, INSTANCE_MAX_NAME_SIZE);
            return 1;
        }
        if (string_equals(name, "engine")) {
            string_copy(value, builder->instance_builder.engine_name, INSTANCE_MAX_NAME_SIZE);
            return 1;
        }
        if (string_equals(name, "version")) {
            uint32_t version[3];
            bool status = string_parse_semantic_version(value, version);
            if (!status) {
                log_warning("Unable to parse app version");
                return 1;
            }
            builder->instance_builder.application_version = VK_MAKE_VERSION(version[0], version[1], version[2]);
            return 1;
        }
        if (string_equals(name, "engine_version")) {
            uint32_t version[3];
            bool status = string_parse_semantic_version(value, version);
            if (!status) {
                log_warning("Unable to parse app engine version");
                return 1;
            }
            builder->instance_builder.engine_version = VK_MAKE_VERSION(version[0], version[1], version[2]);
            return 1;
        }
    }

    if (string_equals(section, "features")) {
        return context_builder_add_feature(builder, name, value, true);
    }
    if (string_equals(section, "optional_features")) {
        return context_builder_add_feature(builder, name, value, false);
    }

    return 1;
}
