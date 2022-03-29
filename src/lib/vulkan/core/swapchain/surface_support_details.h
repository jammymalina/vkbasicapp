#ifndef SURFACE_SUPPORT_DETAILS_H
#define SURFACE_SUPPORT_DETAILS_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../errors.h"
#include "../physical_device/physical_device.h"

#define SURFACE_MAX_FORMATS 16
#define SURFACE_MAX_PRESENT_MODES 16

typedef struct SurfaceSupportDetails {
    const PhysicalDevice* device;

    VkSurfaceCapabilitiesKHR capabilities;

    VkSurfaceFormatKHR formats[SURFACE_MAX_FORMATS];
    uint32_t format_count;

    VkPresentModeKHR present_modes[SURFACE_MAX_PRESENT_MODES];
    uint32_t present_mode_count;
} SurfaceSupportDetails;

static inline void surface_support_details_clear(SurfaceSupportDetails* details) {
    details->device = NULL;
    details->capabilities = (VkSurfaceCapabilitiesKHR){0};
    details->format_count = 0;
    details->present_mode_count = 0;
}

SurfaceSupportError surface_support_details_load(SurfaceSupportDetails* details, const PhysicalDevice* device);
VkSurfaceFormatKHR surface_support_details_find_format(const SurfaceSupportDetails* details,
    const VkSurfaceFormatKHR* desired_formats, uint32_t desired_format_count, VkFormatFeatureFlags feature_flags);
VkExtent2D surface_support_details_find_extent(
    const SurfaceSupportDetails* details, uint32_t desired_width, uint32_t desired_height);
VkPresentModeKHR surface_support_details_find_present_mode(
    const SurfaceSupportDetails* details, const VkPresentModeKHR* desired_present_modes, uint32_t desired_modes_count);

#endif
