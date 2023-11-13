#ifndef X_RENDERERDEVICE_H
#define X_RENDERERDEVICE_H

#include <vulkan/vulkan.h>
#include "../RendererUtil.h"

class RendererDevice {

public:
    Device MainDevice;
    void Create(VkInstance instance);
    void Clean(VkInstance instance);
    SurfaceDetails GetSurfaceDetails(VkPhysicalDevice device);
    QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
    [[nodiscard]] const Device& Get() const { return MainDevice; }
    RendererDevice() {}

private:
    void GetPhysicalDevice(VkInstance instance);
    bool CheckSuitableDevice(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    void CreateLogicalDevice();
};

#endif //X_RENDERERDEVICE_H