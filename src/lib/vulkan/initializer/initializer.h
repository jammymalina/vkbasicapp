#ifndef VULKAN_INITIALIZER_H
#define VULKAN_INITIALIZER_H

#include "../../core/logger/logger.h"
#include "../core/state.h"

void vulkan_state_init(VulkanState* state);
void vulkan_state_destroy(VulkanState* state);

#endif
