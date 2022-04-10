#include "./context_builder.h"

#include "../../../core/memory/memory.h"
#include "../../../core/parsers/ini_parser.h"
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
    const ContextBuilderFeatureItem features[] = {
        CONTEXT_BUILDER_FEATURE_11_ITEM(storageBuffer16BitAccess),
        CONTEXT_BUILDER_FEATURE_11_ITEM(uniformAndStorageBuffer16BitAccess),
        CONTEXT_BUILDER_FEATURE_11_ITEM(storagePushConstant16),
        CONTEXT_BUILDER_FEATURE_11_ITEM(storageInputOutput16),
        CONTEXT_BUILDER_FEATURE_11_ITEM(multiview),
        CONTEXT_BUILDER_FEATURE_11_ITEM(multiviewGeometryShader),
        CONTEXT_BUILDER_FEATURE_11_ITEM(multiviewTessellationShader),
        CONTEXT_BUILDER_FEATURE_11_ITEM(variablePointersStorageBuffer),
        CONTEXT_BUILDER_FEATURE_11_ITEM(variablePointers),
        CONTEXT_BUILDER_FEATURE_11_ITEM(protectedMemory),
        CONTEXT_BUILDER_FEATURE_11_ITEM(samplerYcbcrConversion),
        CONTEXT_BUILDER_FEATURE_11_ITEM(shaderDrawParameters),
    };
    const size_t feature_count = sizeof(features) / sizeof(ContextBuilderFeatureItem);
    const ContextBuilderFeatureItem* feature = context_builder_get_feature_item(name, features, feature_count);
    if (feature == NULL) {
        log_warning("Unknown device extended feature %s", name);
        return 1;
    }

    PhysicalDeviceFeatureItem* feature_item = physical_device_selector_get_extended_required_features_item(
        &builder->device_selector, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES);
    if (feature_item == NULL) {
        VkPhysicalDeviceVulkan11Features features = {0};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        features.pNext = NULL;
        mem_copy(&feature_value, ((byte*)&features) + feature->feature_offset, sizeof(VkBool32));
        physical_device_selector_add_extended_required_features(&builder->device_selector, features);
        return 1;
    }

    mem_copy(&feature_value, ((byte*)feature_item->features) + feature->feature_offset, sizeof(VkBool32));

    return 1;
}

static int context_builder_add_feature_12(ContextBuilder* builder, const char* name, const char* value) {
    VkBool32 feature_value = string_equals(value, "1") ? VK_TRUE : VK_FALSE;
    const ContextBuilderFeatureItem features[] = {
        CONTEXT_BUILDER_FEATURE_12_ITEM(samplerMirrorClampToEdge),
        CONTEXT_BUILDER_FEATURE_12_ITEM(drawIndirectCount),
        CONTEXT_BUILDER_FEATURE_12_ITEM(storageBuffer8BitAccess),
        CONTEXT_BUILDER_FEATURE_12_ITEM(uniformAndStorageBuffer8BitAccess),
        CONTEXT_BUILDER_FEATURE_12_ITEM(storagePushConstant8),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderBufferInt64Atomics),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderSharedInt64Atomics),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderFloat16),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderInt8),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderInputAttachmentArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderUniformTexelBufferArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderStorageTexelBufferArrayDynamicIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderUniformBufferArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderSampledImageArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderStorageBufferArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderStorageImageArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderInputAttachmentArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderUniformTexelBufferArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderStorageTexelBufferArrayNonUniformIndexing),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingUniformBufferUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingSampledImageUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingStorageImageUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingStorageBufferUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingUniformTexelBufferUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingStorageTexelBufferUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingUpdateUnusedWhilePending),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingPartiallyBound),
        CONTEXT_BUILDER_FEATURE_12_ITEM(descriptorBindingVariableDescriptorCount),
        CONTEXT_BUILDER_FEATURE_12_ITEM(runtimeDescriptorArray),
        CONTEXT_BUILDER_FEATURE_12_ITEM(samplerFilterMinmax),
        CONTEXT_BUILDER_FEATURE_12_ITEM(scalarBlockLayout),
        CONTEXT_BUILDER_FEATURE_12_ITEM(imagelessFramebuffer),
        CONTEXT_BUILDER_FEATURE_12_ITEM(uniformBufferStandardLayout),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderSubgroupExtendedTypes),
        CONTEXT_BUILDER_FEATURE_12_ITEM(separateDepthStencilLayouts),
        CONTEXT_BUILDER_FEATURE_12_ITEM(hostQueryReset),
        CONTEXT_BUILDER_FEATURE_12_ITEM(timelineSemaphore),
        CONTEXT_BUILDER_FEATURE_12_ITEM(bufferDeviceAddress),
        CONTEXT_BUILDER_FEATURE_12_ITEM(bufferDeviceAddressCaptureReplay),
        CONTEXT_BUILDER_FEATURE_12_ITEM(bufferDeviceAddressMultiDevice),
        CONTEXT_BUILDER_FEATURE_12_ITEM(vulkanMemoryModel),
        CONTEXT_BUILDER_FEATURE_12_ITEM(vulkanMemoryModelDeviceScope),
        CONTEXT_BUILDER_FEATURE_12_ITEM(vulkanMemoryModelAvailabilityVisibilityChains),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderOutputViewportIndex),
        CONTEXT_BUILDER_FEATURE_12_ITEM(shaderOutputLayer),
        CONTEXT_BUILDER_FEATURE_12_ITEM(subgroupBroadcastDynamicId),
    };
    const size_t feature_count = sizeof(features) / sizeof(ContextBuilderFeatureItem);
    const ContextBuilderFeatureItem* feature = context_builder_get_feature_item(name, features, feature_count);
    if (feature == NULL) {
        log_warning("Unknown device extended feature %s", name);
        return 1;
    }

    PhysicalDeviceFeatureItem* feature_item = physical_device_selector_get_extended_required_features_item(
        &builder->device_selector, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES);
    if (feature_item == NULL) {
        VkPhysicalDeviceVulkan12Features features = {0};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features.pNext = NULL;
        mem_copy(&feature_value, ((byte*)&features) + feature->feature_offset, sizeof(VkBool32));
        physical_device_selector_add_extended_required_features(&builder->device_selector, features);
        return 1;
    }

    mem_copy(&feature_value, ((byte*)feature_item->features) + feature->feature_offset, sizeof(VkBool32));

    return 1;
}

static int context_builder_add_feature_13(ContextBuilder* builder, const char* name, const char* value) {
    VkBool32 feature_value = string_equals(value, "1") ? VK_TRUE : VK_FALSE;
    const ContextBuilderFeatureItem features[] = {
        CONTEXT_BUILDER_FEATURE_13_ITEM(robustImageAccess),
        CONTEXT_BUILDER_FEATURE_13_ITEM(inlineUniformBlock),
        CONTEXT_BUILDER_FEATURE_13_ITEM(descriptorBindingInlineUniformBlockUpdateAfterBind),
        CONTEXT_BUILDER_FEATURE_13_ITEM(pipelineCreationCacheControl),
        CONTEXT_BUILDER_FEATURE_13_ITEM(privateData),
        CONTEXT_BUILDER_FEATURE_13_ITEM(shaderDemoteToHelperInvocation),
        CONTEXT_BUILDER_FEATURE_13_ITEM(shaderTerminateInvocation),
        CONTEXT_BUILDER_FEATURE_13_ITEM(subgroupSizeControl),
        CONTEXT_BUILDER_FEATURE_13_ITEM(computeFullSubgroups),
        CONTEXT_BUILDER_FEATURE_13_ITEM(synchronization2),
        CONTEXT_BUILDER_FEATURE_13_ITEM(textureCompressionASTC_HDR),
        CONTEXT_BUILDER_FEATURE_13_ITEM(shaderZeroInitializeWorkgroupMemory),
        CONTEXT_BUILDER_FEATURE_13_ITEM(dynamicRendering),
        CONTEXT_BUILDER_FEATURE_13_ITEM(shaderIntegerDotProduct),
        CONTEXT_BUILDER_FEATURE_13_ITEM(maintenance4),
    };
    const size_t feature_count = sizeof(features) / sizeof(ContextBuilderFeatureItem);
    const ContextBuilderFeatureItem* feature = context_builder_get_feature_item(name, features, feature_count);
    if (feature == NULL) {
        log_warning("Unknown device extended feature %s", name);
        return 1;
    }

    PhysicalDeviceFeatureItem* feature_item = physical_device_selector_get_extended_required_features_item(
        &builder->device_selector, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES);
    if (feature_item == NULL) {
        VkPhysicalDeviceVulkan13Features features = {0};
        features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features.pNext = NULL;
        mem_copy(&feature_value, ((byte*)&features) + feature->feature_offset, sizeof(VkBool32));
        physical_device_selector_add_extended_required_features(&builder->device_selector, features);
        return 1;
    }

    mem_copy(&feature_value, ((byte*)feature_item->features) + feature->feature_offset, sizeof(VkBool32));

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

    PhysicalDeviceError phys_dev_status =
        physical_device_selector_select(&builder->device_selector, &context->physical_device);
    physical_device_selector_destroy(&builder->device_selector);
    ASSERT_SUCCESS_LOG(phys_dev_status, PhysicalDeviceError, physical_device_error_to_string, CONTEXT_INIT_ERROR);

    builder->device_builder.physical_device = &context->physical_device;

    DeviceError device_status = device_builder_build(&builder->device_builder, &context->device);
    ASSERT_SUCCESS_LOG(device_status, DeviceError, device_error_to_string, CONTEXT_INIT_ERROR);

    return CONTEXT_SUCCESS;
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

    if (string_equals(section, "device")) {
        if (string_equals(name, "experimental_feature_validation_enabled")) {
            builder->device_selector.experimental_feature_validation_enabled = string_equals(value, "1");
            return 1;
        }
        if (string_equals(name, "type")) {
            if (string_equals(value, "any")) {
                builder->device_selector.allow_any_type = true;
                return 1;
            }
            builder->device_selector.preferred_device_type = preferred_device_type_from_string(value);
            builder->device_selector.allow_any_type = false;
            return 1;
        }
        if (string_equals(name, "use_first_gpu_unconditionally")) {
            builder->device_selector.use_first_gpu_unconditionally = string_equals(value, "1");
            return 1;
        }
        if (string_equals(name, "enable_portability_subset")) {
            builder->device_selector.enable_portability_subset = string_equals(value, "1");
            return 1;
        }
        if (string_equals(name, "memory_size_MB")) {
            INI_PARSER_ASSERT_INT(section, name, value, false, 1);
            VkDeviceSize mem_size = string_to_int(value, VkDeviceSize);
            builder->device_selector.required_mem_size = MB_TO_BYTES(mem_size);
            return 1;
        }
    }

    if (string_equals(section, "extensions")) {
        bool enabled = string_equals(value, "1");
        if (!physical_device_selector_set_required_extension(&builder->device_selector, name, enabled)) {
            log_warning("Unable to add extension %s", name);
        }
        return 1;
    }
    if (string_equals(section, "optional_extensions")) {
        bool enabled = string_equals(value, "1");
        if (!physical_device_selector_set_desired_extension(&builder->device_selector, name, enabled)) {
            log_warning("Unable to add optional extension %s", name);
        }
        return 1;
    }

    if (string_equals(section, "features")) {
        return context_builder_add_feature(builder, name, value, true);
    }
    if (string_equals(section, "optional_features")) {
        return context_builder_add_feature(builder, name, value, false);
    }
    if (string_equals(section, "features_11")) {
        return context_builder_add_feature_11(builder, name, value);
    }
    if (string_equals(section, "features_12")) {
        return context_builder_add_feature_12(builder, name, value);
    }
    if (string_equals(section, "features_13")) {
        return context_builder_add_feature_13(builder, name, value);
    }

    return 1;
}
