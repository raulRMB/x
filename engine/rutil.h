//
// Created by Raul Romero on 2023-09-23.
//

#ifndef X_RUTIL_H
#define X_RUTIL_H

#include <base/defines.h>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace xRUtil
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char*> DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct QueueFamilyIndices
    {
        i32 GraphicsFamily = -1;
        i32 PresentationFamily = -1;

        [[nodiscard]] bool IsValid() const
        {
            return GraphicsFamily >= 0 && PresentationFamily >= 0;
        }
    };

    struct SwapChainDetails
    {
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentationModes;
    };

    struct SwapChainImage
    {
        VkImage Image;
        VkImageView ImageView;
    };

};


#endif //X_RUTIL_H
