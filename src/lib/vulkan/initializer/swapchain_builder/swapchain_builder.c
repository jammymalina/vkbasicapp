#include "./swapchain_builder.h"

#include "../../core/instance.h"
#include "../../core/physical_device/physical_device.h"
#include "../../core/swapchain/surface_support_details.h"
#include "../../utils/queue.h"

static SwapchainError swapchain_builder_validate(const SwapchainBuilder* builder) {
    if (builder->device == NULL) {
        return NO_DEVICE_PROVIDED_SWAPCHAIN;
    }

    return SWAPCHAIN_NO_ERROR;
}

SwapchainError swapchain_builder_build(SwapchainBuilder* builder, Swapchain* swapchain) {
    SwapchainError status = swapchain_builder_validate(builder);
    ASSERT_NO_ERROR(status, status);

    const PhysicalDevice* physical_device = builder->device->physical_device;
    const Instance* instance = physical_device->instance;

    if (builder->combined_queue == UINT32_MAX) {
        builder->combined_queue = queue_utils_get_present_queue_index(physical_device, instance);
    }

    if (builder->desired_format_count == 0) {
        swapchain_builder_add_default_formats(builder);
    }
    if (builder->desired_present_mode_count == 0) {
        swapchain_builder_add_default_present_modes(builder);
    }

    SurfaceSupportDetails surface_support;
    SurfaceSupportError surface_status = surface_support_details_load(&surface_support, physical_device);
    ASSERT_NO_ERROR(surface_status, FAILED_QUERY_SURFACE_SUPPORT_DETAILS);

    uint32_t image_count = surface_support.capabilities.minImageCount + 1;
    if (surface_support.capabilities.maxImageCount > 0 && image_count > surface_support.capabilities.maxImageCount) {
        image_count = surface_support.capabilities.maxImageCount;
    }
    if (image_count > SWAPCHAIN_MAX_IMAGES) {
        return TOO_MANY_IMAGES_REQUESTED;
    }

    uint32_t image_array_layers = builder->array_layer_count;
    if (surface_support.capabilities.maxImageArrayLayers < builder->array_layer_count) {
        image_array_layers = surface_support.capabilities.maxImageArrayLayers;
    }
    if (builder->array_layer_count == 0) {
        image_array_layers = 1;
    }

    VkSurfaceFormatKHR surface_format = surface_support_details_find_format(
        &surface_support, builder->desired_formats, builder->desired_format_count, builder->format_feature_flags);
    VkPresentModeKHR present_mode = surface_support_details_find_present_mode(
        &surface_support, builder->desired_present_modes, builder->desired_present_mode_count);
    VkExtent2D extent =
        surface_support_details_find_extent(&surface_support, builder->desired_width, builder->desired_height);

    VkSurfaceTransformFlagBitsKHR pre_transform = builder->pre_transform;
    if (builder->pre_transform == (VkSurfaceTransformFlagBitsKHR){0}) {
        pre_transform = surface_support.capabilities.currentTransform;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = builder->flags,
        .surface = instance->surface,
        .minImageCount = image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = image_array_layers,
        .imageUsage = builder->image_usage_flags,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &builder->combined_queue,
        .preTransform = pre_transform,
        .compositeAlpha = builder->composite_alpha,
        .presentMode = present_mode,
        .clipped = builder->clipped,
        .oldSwapchain = builder->old_swapchain,
    };

    VkSwapchainKHR handle;
    VkResult create_status = vkCreateSwapchainKHR(builder->device->handle, &swapchain_create_info, NULL, &handle);
    ASSERT_VULKAN_STATUS(create_status, "Unable to create swapchain", FAILED_CREATE_SWAPCHAIN);

    swapchain->handle = handle;
    swapchain->device = builder->device;
    swapchain->image_count = image_count;
    swapchain->image_format = surface_format.format;
    swapchain->extent = extent;
    swapchain->queue_index = builder->combined_queue;

    status = swapchain_load_images(swapchain);
    ASSERT_NO_ERROR(status, status);
    status = swapchain_load_image_views(swapchain);
    ASSERT_NO_ERROR(status, status);

    return SWAPCHAIN_NO_ERROR;
}

bool swapchain_builder_add_desired_format(SwapchainBuilder* builder, VkSurfaceFormatKHR format) {
    if (builder->desired_format_count >= SURFACE_MAX_FORMATS) {
        return false;
    }
    builder->desired_formats[builder->desired_format_count] = format;
    builder->desired_format_count += 1;
    return true;
}

bool swapchain_builder_add_default_formats(SwapchainBuilder* builder) {
    bool status = swapchain_builder_add_desired_format(
                      builder, (VkSurfaceFormatKHR){VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}) &&
                  swapchain_builder_add_desired_format(
                      builder, (VkSurfaceFormatKHR){VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
    return status;
}

bool swapchain_builder_add_desired_present_mode(SwapchainBuilder* builder, VkPresentModeKHR present_mode) {
    if (builder->desired_present_mode_count >= SURFACE_MAX_PRESENT_MODES) {
        return false;
    }
    builder->desired_present_modes[builder->desired_present_mode_count] = present_mode;
    builder->desired_present_mode_count += 1;
    return true;
}

bool swapchain_builder_add_default_present_modes(SwapchainBuilder* builder) {
    bool status = swapchain_builder_add_desired_present_mode(builder, VK_PRESENT_MODE_MAILBOX_KHR) &&
                  swapchain_builder_add_desired_present_mode(builder, VK_PRESENT_MODE_FIFO_KHR);
    return status;
}
