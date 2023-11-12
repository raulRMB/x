#pragma once

#include "../../../base/defines.h"
#include "../../window.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>
#include "../../rutil.h"

namespace x {

struct Device
{
    VkPhysicalDevice PhysicalDevice;
    VkDevice LogicalDevice;
    VkSurfaceKHR Surface;
    VkQueue GraphicsQueue;
    VkQueue PresentationQueue;
    VkDeviceSize MinUniformBufferOffset;
};

class rendererDevice {

public:
    Device MainDevice;
    void create(VkInstance Instance);
    void clean(VkInstance Instance);
    x::RenderUtil::SurfaceDetails GetSurfaceDetails(VkPhysicalDevice device);
    x::RenderUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
    Device get() { return MainDevice; }
    rendererDevice() {}

private:
    void GetPhysicalDevice(VkInstance Instance);
    bool CheckSuitableDevice(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    void CreateLogicalDevice(VkInstance Instance);
};
}