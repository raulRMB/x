#pragma once

#include <vulkan/vulkan.h>
#include "../RendererUtil.h"

namespace x {

class RendererDevice {

public:
    RenderUtil::Device MainDevice;
    void Create(VkInstance instance);
    void Clean(VkInstance instance);
    RenderUtil::SurfaceDetails GetSurfaceDetails(VkPhysicalDevice device);
    RenderUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
    [[nodiscard]] const RenderUtil::Device& Get() const { return MainDevice; }
    RendererDevice() {}

private:
    void GetPhysicalDevice(VkInstance instance);
    bool CheckSuitableDevice(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    void CreateLogicalDevice();
};

}