#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "../../core/logger/logger.h"
#include "../core/errors.h"
#include "../core/state.h"

void vulkan_state_init(VulkanState* state, void* window_handle);
SwapchainError vulkan_state_create_swapchain(VulkanState* state);
void vulkan_state_destroy(VulkanState* state);

#endif
