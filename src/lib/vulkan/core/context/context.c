#include "./context.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../../initializer/device_builder/device_builder.h"
#include "../../initializer/function_loader/function_loader.h"
#include "../../initializer/instance_builder/instance_builder.h"
#include "../../initializer/physical_device_selector/physical_device_selector.h"
#include "../errors.h"
#include "../functions.h"
#include "../instance.h"
#include "../library.h"

ContextError context_init(Context* context, void* window_handle) {
    // TODO: Read vulkan configuration from ini file
    ASSERT_NO_ERROR_LOG(library_load(&context->library), LibraryError, library_error_to_string, CONTEXT_INIT_ERROR);
    function_loader_load_external_function((PFN_vkGetInstanceProcAddr)context->library.load_function);
    ASSERT_NO_ERROR_LOG(function_loader_load_global_functions(), FunctionLoaderError, function_loader_error_to_string,
        CONTEXT_INIT_ERROR);

    if (!system_info_init(&context->system)) {
        return CONTEXT_INIT_ERROR;
    }

    InstanceBuilder instance_builder = {0};
    instance_builder_clear(&instance_builder);
    instance_builder.system = &context->system;
    instance_builder.window_handle = window_handle;
    instance_builder.app_name = "Basic app";
    instance_builder.application_version = VK_MAKE_VERSION(1, 0, 0);
    instance_builder.engine_version = VK_MAKE_VERSION(1, 0, 0);
    instance_builder.engine_name = "jammyengine";
#ifdef DEBUG
    instance_builder.debug_enabled = true;
    instance_builder_add_layer(&instance_builder, "VK_LAYER_KHRONOS_validation");
#endif
    ASSERT_NO_ERROR_LOG(instance_builder_build(&instance_builder, &context->instance), InstanceError,
        instance_error_to_string, CONTEXT_INIT_ERROR);

    PhysicalDeviceSelector device_selector = {0};
    physical_device_selector_clear(&device_selector);
    device_selector.instance = &context->instance;
    device_selector.enabled_experimental_feature_validation = true;

    // enable dynamic rendering
    VkPhysicalDeviceVulkan13Features features_13 = {0};
    features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_13.dynamicRendering = VK_TRUE;
    physical_device_selector_add_extended_required_features(&device_selector, features_13);
    physical_device_selector_add_required_extension(&device_selector, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

    PhysicalDeviceError phys_dev_status = physical_device_selector_select(&device_selector, &context->physical_device);
    physical_device_selector_destroy(&device_selector);
    ASSERT_NO_ERROR_LOG(phys_dev_status, PhysicalDeviceError, physical_device_error_to_string, CONTEXT_INIT_ERROR);

    DeviceBuilder device_builder = {0};
    device_builder_clear(&device_builder);
    device_builder.physical_device = &context->physical_device;

    DeviceError device_status = device_builder_build(&device_builder, &context->device);
    ASSERT_NO_ERROR_LOG(device_status, DeviceError, device_error_to_string, CONTEXT_INIT_ERROR);

    return CONTEXT_NO_ERROR;
}

void context_destroy(Context* context) {
    device_destroy(&context->device);
    instance_destroy(&context->instance);
    system_info_destroy(&context->system);
    library_unload(&context->library);
    context_clear(context);
}
