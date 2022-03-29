#include "./initializer.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../core/errors.h"
#include "../core/functions.h"
#include "../core/instance.h"
#include "../core/library.h"
#include "./device_builder/device_builder.h"
#include "./function_loader/function_loader.h"
#include "./instance_builder/instance_builder.h"
#include "./physical_device_selector/physical_device_selector.h"
#include "./swapchain_builder/swapchain_builder.h"

void vulkan_state_init(VulkanState* state, void* window_handle) {
    if (state->is_init) {
        return;
    }
    // TODO: Read vulkan configuration from ini file
    ASSERT_NO_ERROR_LOG(library_load(&state->library), LibraryError, library_error_to_string);
    function_loader_load_external_function((PFN_vkGetInstanceProcAddr)state->library.load_function);
    ASSERT_NO_ERROR_LOG(function_loader_load_global_functions(), FunctionLoaderError, function_loader_error_to_string);

    if (!system_info_init(&state->system)) {
        return;
    }

    InstanceBuilder instance_builder = {0};
    instance_builder_clear(&instance_builder);
    instance_builder.system = &state->system;
    instance_builder.window_handle = window_handle;
    instance_builder.app_name = "Basic app";
    instance_builder.application_version = VK_MAKE_VERSION(1, 0, 0);
    instance_builder.engine_version = VK_MAKE_VERSION(1, 0, 0);
    instance_builder.engine_name = "jammyengine";
#ifdef DEBUG
    instance_builder.debug_enabled = true;
    instance_builder_add_layer(&instance_builder, "VK_LAYER_KHRONOS_validation");
#endif
    ASSERT_NO_ERROR_LOG(
        instance_builder_build(&instance_builder, &state->instance), InstanceError, instance_error_to_string);

    PhysicalDeviceSelector device_selector = {0};
    physical_device_selector_clear(&device_selector);
    device_selector.instance = &state->instance;
    device_selector.enabled_experimental_feature_validation = true;

    // enable dynamic rendering
    VkPhysicalDeviceVulkan13Features features_13 = {0};
    features_13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features_13.dynamicRendering = VK_TRUE;
    physical_device_selector_add_extended_required_features(&device_selector, features_13);
    physical_device_selector_add_required_extension(&device_selector, "VK_KHR_dynamic_rendering");

    PhysicalDeviceError device_status = physical_device_selector_select(&device_selector, &state->physical_device);
    physical_device_selector_destroy(&device_selector);
    ASSERT_NO_ERROR_LOG(device_status, PhysicalDeviceError, physical_device_error_to_string);

    DeviceBuilder device_builder = {0};
    device_builder_clear(&device_builder);
    device_builder.physical_device = &state->physical_device;

    device_builder_build(&device_builder, &state->device);

    SwapchainError swapchain_status = vulkan_state_create_swapchain(state);
    ASSERT_NO_ERROR_LOG(swapchain_status, SwapchainError, swapchain_error_to_string);

    state->is_init = true;
}

SwapchainError vulkan_state_create_swapchain(VulkanState* state) {
    if (!device_is_init(&state->device)) {
        return NO_DEVICE_PROVIDED_SWAPCHAIN;
    }

    Swapchain swapchain;

    SwapchainBuilder swapchain_builder = {0};
    swapchain_builder.old_swapchain = state->swapchain.handle;
    swapchain_builder_clear(&swapchain_builder);
    swapchain_builder.device = &state->device;

    SwapchainError status = swapchain_builder_build(&swapchain_builder, &swapchain);
    ASSERT_NO_ERROR(status, status);

    swapchain_destroy(&state->swapchain);
    swapchain_copy(&swapchain, &state->swapchain);

    return SWAPCHAIN_NO_ERROR;
}

void vulkan_state_destroy(VulkanState* state) {
    swapchain_destroy(&state->swapchain);
    device_destroy(&state->device);
    instance_destroy(&state->instance);
    system_info_destroy(&state->system);
    library_unload(&state->library);
    state->is_init = false;
}
