#pragma once

#include <vulkan/vulkan.h>
#include "..\RendererUtil.h"

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

class RendererDevice {

public:
    Device MainDevice;
    void Create(VkInstance instance);
    void Clean(VkInstance instance);
    RenderUtil::SurfaceDetails GetSurfaceDetails(VkPhysicalDevice device);
    RenderUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
    [[nodiscard]] const Device& Get() const { return MainDevice; }
    RendererDevice() {}

private:
    void GetPhysicalDevice(VkInstance instance);
    bool CheckSuitableDevice(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    void CreateLogicalDevice();
};

}