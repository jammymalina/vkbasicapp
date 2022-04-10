#include "./context.h"

void context_destroy(Context* context) {
    device_destroy(&context->device);
    instance_destroy(&context->instance);
    system_info_destroy(&context->system);
    library_unload(&context->library);
    context_clear(context);
}
