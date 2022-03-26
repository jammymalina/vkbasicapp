#include "./initializer.h"

#include "../core/errors.h"
#include "../core/functions.h"
#include "../core/instance.h"
#include "../core/library.h"
#include "./function_loader/function_loader.h"
#include "./instance_builder/instance_builder.h"

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
    system_info_display(&state->system);

    InstanceBuilder instance_builder = instance_builder_create();
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

    state->is_init = true;
}

void vulkan_state_destroy(VulkanState* state) {
    instance_destroy(&state->instance);
    system_info_destroy(&state->system);
    library_unload(&state->library);
    state->is_init = false;
}
