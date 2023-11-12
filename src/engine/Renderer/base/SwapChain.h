#include "../../../base/defines.h"
#include <vulkan/vulkan.h>
#include "../RendererUtil.h"

namespace x {

class SwapChain {

public:
    VkSwapchainKHR Swapchain;
    VkFormat SwapchainImageFormat;
    VkExtent2D SwapchainExtent;
    std::vector<x::RenderUtil::SwapChainImage> SwapchainImages;
    std::vector<VkFramebuffer> SwapchainFramebuffers;

    void Clean(RenderUtil::Device MainDevice);
    void Create(RenderUtil::Device MainDevice, RenderUtil::SurfaceDetails surfaceDetails, RenderUtil::QueueFamilyIndices indices);

private:
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);
    VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
    VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes);

};
}