#ifndef EXPORTED_VULKAN_FUNCTION
#define EXPORTED_VULKAN_FUNCTION(function)
#endif

EXPORTED_VULKAN_FUNCTION(vkGetInstanceProcAddr)

#undef EXPORTED_VULKAN_FUNCTION
//
#ifndef GLOBAL_LEVEL_VK_FUNCTION
#define GLOBAL_LEVEL_VK_FUNCTION(function)
#endif

GLOBAL_LEVEL_VK_FUNCTION(vkEnumerateInstanceExtensionProperties)
GLOBAL_LEVEL_VK_FUNCTION(vkEnumerateInstanceLayerProperties)
GLOBAL_LEVEL_VK_FUNCTION(vkCreateInstance)

#undef GLOBAL_LEVEL_VK_FUNCTION
//
#ifndef INSTANCE_LEVEL_VK_FUNCTION
#define INSTANCE_LEVEL_VK_FUNCTION(function)
#endif

INSTANCE_LEVEL_VK_FUNCTION(vkEnumeratePhysicalDevices)
INSTANCE_LEVEL_VK_FUNCTION(vkEnumerateDeviceExtensionProperties)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceFeatures)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceFeatures2)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceProperties)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceMemoryProperties)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceFormatProperties)
INSTANCE_LEVEL_VK_FUNCTION(vkGetPhysicalDeviceImageFormatProperties)
INSTANCE_LEVEL_VK_FUNCTION(vkCreateDevice)
INSTANCE_LEVEL_VK_FUNCTION(vkGetDeviceProcAddr)
INSTANCE_LEVEL_VK_FUNCTION(vkDestroyInstance)

#undef INSTANCE_LEVEL_VK_FUNCTION
//
#ifndef INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION
#define INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(function)
#endif

#ifdef DEBUG

INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkCreateDebugUtilsMessengerEXT)
INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkDestroyDebugUtilsMessengerEXT)

#endif

INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceSupportKHR)
INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfaceFormatsKHR)
INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkGetPhysicalDeviceSurfacePresentModesKHR)
INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkDestroySurfaceKHR)

#undef INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION
//
#ifndef DEVICE_LEVEL_VK_FUNCTION
#define DEVICE_LEVEL_VK_FUNCTION(function)
#endif

DEVICE_LEVEL_VK_FUNCTION(vkGetDeviceQueue)
DEVICE_LEVEL_VK_FUNCTION(vkDeviceWaitIdle)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyDevice)
DEVICE_LEVEL_VK_FUNCTION(vkCreateBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkGetBufferMemoryRequirements)
DEVICE_LEVEL_VK_FUNCTION(vkAllocateMemory)
DEVICE_LEVEL_VK_FUNCTION(vkBindBufferMemory)
DEVICE_LEVEL_VK_FUNCTION(vkCmdPipelineBarrier)
DEVICE_LEVEL_VK_FUNCTION(vkCreateImage)
DEVICE_LEVEL_VK_FUNCTION(vkGetImageMemoryRequirements)
DEVICE_LEVEL_VK_FUNCTION(vkBindImageMemory)
DEVICE_LEVEL_VK_FUNCTION(vkCreateImageView)
DEVICE_LEVEL_VK_FUNCTION(vkMapMemory)
DEVICE_LEVEL_VK_FUNCTION(vkFlushMappedMemoryRanges)
DEVICE_LEVEL_VK_FUNCTION(vkUnmapMemory)
DEVICE_LEVEL_VK_FUNCTION(vkCmdCopyBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkCmdCopyBufferToImage)
DEVICE_LEVEL_VK_FUNCTION(vkCmdCopyImageToBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkBeginCommandBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkEndCommandBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkQueueSubmit)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyImageView)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyImage)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkFreeMemory)
DEVICE_LEVEL_VK_FUNCTION(vkCreateCommandPool)
DEVICE_LEVEL_VK_FUNCTION(vkAllocateCommandBuffers)
DEVICE_LEVEL_VK_FUNCTION(vkCreateSemaphore)
DEVICE_LEVEL_VK_FUNCTION(vkCreateFence)
DEVICE_LEVEL_VK_FUNCTION(vkWaitForFences)
DEVICE_LEVEL_VK_FUNCTION(vkResetFences)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyFence)
DEVICE_LEVEL_VK_FUNCTION(vkDestroySemaphore)
DEVICE_LEVEL_VK_FUNCTION(vkResetCommandBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkFreeCommandBuffers)
DEVICE_LEVEL_VK_FUNCTION(vkResetCommandPool)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyCommandPool)
DEVICE_LEVEL_VK_FUNCTION(vkCreateBufferView)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyBufferView)
DEVICE_LEVEL_VK_FUNCTION(vkQueueWaitIdle)
DEVICE_LEVEL_VK_FUNCTION(vkCreateSampler)
DEVICE_LEVEL_VK_FUNCTION(vkCreateDescriptorSetLayout)
DEVICE_LEVEL_VK_FUNCTION(vkCreateDescriptorPool)
DEVICE_LEVEL_VK_FUNCTION(vkAllocateDescriptorSets)
DEVICE_LEVEL_VK_FUNCTION(vkUpdateDescriptorSets)
DEVICE_LEVEL_VK_FUNCTION(vkCmdBindDescriptorSets)
DEVICE_LEVEL_VK_FUNCTION(vkFreeDescriptorSets)
DEVICE_LEVEL_VK_FUNCTION(vkResetDescriptorPool)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyDescriptorPool)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyDescriptorSetLayout)
DEVICE_LEVEL_VK_FUNCTION(vkDestroySampler)
DEVICE_LEVEL_VK_FUNCTION(vkCreateRenderPass)
DEVICE_LEVEL_VK_FUNCTION(vkCreateFramebuffer)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyFramebuffer)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyRenderPass)
DEVICE_LEVEL_VK_FUNCTION(vkCmdBeginRenderPass)
DEVICE_LEVEL_VK_FUNCTION(vkCmdNextSubpass)
DEVICE_LEVEL_VK_FUNCTION(vkCmdEndRenderPass)
DEVICE_LEVEL_VK_FUNCTION(vkCreatePipelineCache)
DEVICE_LEVEL_VK_FUNCTION(vkGetPipelineCacheData)
DEVICE_LEVEL_VK_FUNCTION(vkMergePipelineCaches)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyPipelineCache)
DEVICE_LEVEL_VK_FUNCTION(vkCreateGraphicsPipelines)
DEVICE_LEVEL_VK_FUNCTION(vkCreateComputePipelines)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyPipeline)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyEvent)
DEVICE_LEVEL_VK_FUNCTION(vkCreateQueryPool)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyQueryPool)
DEVICE_LEVEL_VK_FUNCTION(vkGetQueryPoolResults)
DEVICE_LEVEL_VK_FUNCTION(vkCreateShaderModule)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyShaderModule)
DEVICE_LEVEL_VK_FUNCTION(vkCreatePipelineLayout)
DEVICE_LEVEL_VK_FUNCTION(vkDestroyPipelineLayout)
DEVICE_LEVEL_VK_FUNCTION(vkCmdBindPipeline)
DEVICE_LEVEL_VK_FUNCTION(vkCmdSetViewport)
DEVICE_LEVEL_VK_FUNCTION(vkCmdSetScissor)
DEVICE_LEVEL_VK_FUNCTION(vkCmdBindVertexBuffers)
DEVICE_LEVEL_VK_FUNCTION(vkCmdDraw)
DEVICE_LEVEL_VK_FUNCTION(vkCmdDrawIndexed)
DEVICE_LEVEL_VK_FUNCTION(vkCmdDispatch)
DEVICE_LEVEL_VK_FUNCTION(vkCmdCopyImage)
DEVICE_LEVEL_VK_FUNCTION(vkCmdPushConstants)
DEVICE_LEVEL_VK_FUNCTION(vkCmdClearColorImage)
DEVICE_LEVEL_VK_FUNCTION(vkCmdClearDepthStencilImage)
DEVICE_LEVEL_VK_FUNCTION(vkCmdBindIndexBuffer)
DEVICE_LEVEL_VK_FUNCTION(vkCmdSetLineWidth)
DEVICE_LEVEL_VK_FUNCTION(vkCmdSetDepthBias)
DEVICE_LEVEL_VK_FUNCTION(vkCmdSetBlendConstants)
DEVICE_LEVEL_VK_FUNCTION(vkCmdExecuteCommands)
DEVICE_LEVEL_VK_FUNCTION(vkCmdClearAttachments)
DEVICE_LEVEL_VK_FUNCTION(vkCmdResetQueryPool)
DEVICE_LEVEL_VK_FUNCTION(vkCmdWriteTimestamp)

#undef DEVICE_LEVEL_VK_FUNCTION
//
#ifndef DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION
#define DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(function, extension)
#endif

DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkCreateSwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkGetSwapchainImagesKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkAcquireNextImageKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkQueuePresentKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)
DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkDestroySwapchainKHR, VK_KHR_SWAPCHAIN_EXTENSION_NAME)

DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkCmdBeginRenderingKHR, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(vkCmdEndRenderingKHR, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)

#undef DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION
