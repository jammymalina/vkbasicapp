#ifndef VULKAN_FUNCTION_LOADER_H
#define VULKAN_FUNCTION_LOADER_H

#include <vulkan/vulkan.h>

#include "../../core/errors.h"

void function_loader_load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc);
FunctionLoaderError function_loader_load_global_functions();
FunctionLoaderError function_loader_load_instance_vulkan_functions(VkInstance instance);
FunctionLoaderError function_loader_load_device_level_functions(VkDevice device);

#endif
