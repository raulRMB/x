#ifndef X_SWAPCHAIN_H
#define X_SWAPCHAIN_H

#include "../../Core/defines.h"
#include <vulkan/vulkan.h>
#include "../RendererUtil.h"

class SwapChain {

public:
    VkSwapchainKHR Swapchain;
    VkFormat SwapchainImageFormat;
    VkExtent2D SwapchainExtent;
    std::vector<SwapChainImage> SwapchainImages;
    std::vector<VkFramebuffer> SwapchainFramebuffers;

    void Clean(Device MainDevice);
    void Create(Device MainDevice, SurfaceDetails surfaceDetails, QueueFamilyIndices indices);

private:
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
    VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes);

};

#endif //X_SWAPCHAIN_H