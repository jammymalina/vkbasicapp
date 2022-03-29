#include "./surface_support_details.h"

#include <stdint.h>

#include "../../../core/utils/macro.h"
#include "../errors.h"
#include "../functions.h"

SurfaceSupportError surface_support_details_load(SurfaceSupportDetails* details, const PhysicalDevice* device) {
    surface_support_details_clear(details);
    VkSurfaceKHR surface = device->instance->surface;
    if (surface == VK_NULL_HANDLE) {
        return SURFACE_HANDLE_NULL;
    }

    VkSurfaceCapabilitiesKHR capabilities;
    ASSERT_VULKAN_STATUS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->handle, surface, &capabilities),
        "Unable to load surface capabilities", FAILED_GET_SURFACE_CAPABILITIES);

    uint32_t format_count, present_mode_count;
    ASSERT_VULKAN_STATUS(vkGetPhysicalDeviceSurfaceFormatsKHR(device->handle, surface, &format_count, NULL),
        "Unable to load surface formats", FAILED_ENUMERATE_SURFACE_FORMATS);
    ASSERT_VULKAN_STATUS(vkGetPhysicalDeviceSurfacePresentModesKHR(device->handle, surface, &present_mode_count, NULL),
        "Unable to load surface present modes", FAILED_ENUMERATE_PRESENT_MODES);
    if (format_count > SURFACE_MAX_FORMATS) {
        return TOO_MANY_SURFACE_FORMATS_REQUESTED;
    }
    if (present_mode_count > SURFACE_MAX_PRESENT_MODES) {
        return TOO_MANY_SURFACE_PRESENT_MODES_REQUESTED;
    }

    VkSurfaceFormatKHR formats[format_count];
    VkPresentModeKHR present_modes[present_mode_count];

    ASSERT_VULKAN_STATUS(vkGetPhysicalDeviceSurfaceFormatsKHR(device->handle, surface, &format_count, formats),
        "Unable to load surface formats", FAILED_ENUMERATE_SURFACE_FORMATS);
    ASSERT_VULKAN_STATUS(
        vkGetPhysicalDeviceSurfacePresentModesKHR(device->handle, surface, &present_mode_count, present_modes),
        "Unable to load surface present modes", FAILED_ENUMERATE_PRESENT_MODES);

    details->device = device;
    details->capabilities = capabilities;
    details->format_count = format_count;
    details->present_mode_count = present_mode_count;

    for (uint32_t i = 0; i < details->format_count; i++) {
        details->formats[i] = formats[i];
    }
    for (uint32_t i = 0; i < details->present_mode_count; i++) {
        details->present_modes[i] = present_modes[i];
    }

    return SURFACE_SUPPORT_NO_ERROR;
}

VkSurfaceFormatKHR surface_support_details_find_format(const SurfaceSupportDetails* details,
    const VkSurfaceFormatKHR* desired_formats, uint32_t desired_format_count, VkFormatFeatureFlags feature_flags) {
    if (details->device == NULL || details->format_count == 0) {
        return (VkSurfaceFormatKHR){0};
    }
    for (uint32_t desired_index = 0; desired_index < desired_format_count; desired_index++) {
        for (uint32_t avail_index = 0; avail_index < details->format_count; avail_index++) {
            VkSurfaceFormatKHR desired_format = desired_formats[desired_index];
            VkSurfaceFormatKHR avail_format = details->formats[avail_index];
            if (desired_format.format == avail_format.format && desired_format.colorSpace == avail_format.colorSpace) {
                VkFormatProperties properties;
                vkGetPhysicalDeviceFormatProperties(details->device->handle, desired_format.format, &properties);
                if ((properties.optimalTilingFeatures & feature_flags) == feature_flags) {
                    return desired_format;
                }
            }
        }
    }

    return details->formats[0];
}

VkExtent2D surface_support_details_find_extent(
    const SurfaceSupportDetails* details, uint32_t desired_width, uint32_t desired_height) {
    if (details->capabilities.currentExtent.width != UINT32_MAX) {
        return details->capabilities.currentExtent;
    }

    VkExtent2D actual_extent = {desired_width, desired_height};

    actual_extent.width = MAX(details->capabilities.minImageExtent.width,
        MIN(details->capabilities.maxImageExtent.width, actual_extent.width));
    actual_extent.height = MAX(details->capabilities.minImageExtent.height,
        MIN(details->capabilities.maxImageExtent.height, actual_extent.height));

    return actual_extent;
}

VkPresentModeKHR surface_support_details_find_present_mode(
    const SurfaceSupportDetails* details, const VkPresentModeKHR* desired_present_modes, uint32_t desired_modes_count) {
    for (uint32_t desired_index = 0; desired_index < desired_modes_count; desired_index++) {
        for (uint32_t avail_index = 0; avail_index < details->present_mode_count; avail_index++) {
            if (desired_present_modes[desired_index] == details->present_modes[avail_index]) {
                return desired_present_modes[desired_index];
            }
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
