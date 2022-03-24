#include "./initializer.h"

#include "../core/errors.h"
#include "../core/instance.h"
#include "../core/library.h"
#include "./function_loader/function_loader.h"

void vulkan_state_init(VulkanState* state) {
    if (state->is_init) {
        return;
    }

    ASSERT_NO_ERROR(library_load(&state->library), LibraryError);
    function_loader_load_external_function((PFN_vkGetInstanceProcAddr)&state->library.load_function);

    ASSERT_NO_ERROR(instance_init(&state->instance), InstanceError);

    state->is_init = true;
}

void vulkan_state_destroy(VulkanState* state) {
    instance_destroy(&state->instance);
    library_unload(&state->library);
}
