#include "../../Core/defines.h"
#include <vulkan/vulkan.h>
#include "../RendererUtil.h"
#include "SwapChain.h"

namespace x {

    void SwapChain::Clean(RenderUtil::Device MainDevice) 
    {
        for(const x::RenderUtil::SwapChainImage& swapChainImage : SwapchainImages) {
            vkDestroyImageView(MainDevice.LogicalDevice, swapChainImage.ImageView, nullptr);
        }
        for(const VkFramebuffer& framebuffer : SwapchainFramebuffers) {
            vkDestroyFramebuffer(MainDevice.LogicalDevice, framebuffer, nullptr);
        }
        vkDestroySwapchainKHR(MainDevice.LogicalDevice, Swapchain, nullptr);
    }

    void SwapChain::Create(RenderUtil::Device MainDevice, RenderUtil::SurfaceDetails surfaceDetails, RenderUtil::QueueFamilyIndices indices)
    {
        VkSurfaceFormatKHR surfaceFormat = ChooseBestSurfaceFormat(surfaceDetails.Formats);
        VkPresentModeKHR presentMode = ChooseBestPresentationMode(surfaceDetails.PresentationModes);
        VkExtent2D extent = ChooseSwapExtent(surfaceDetails.SurfaceCapabilities);

        u32 imageCount = surfaceDetails.SurfaceCapabilities.minImageCount + 1;

        if(surfaceDetails.SurfaceCapabilities.maxImageCount > 0
            && surfaceDetails.SurfaceCapabilities.maxImageCount < imageCount)
        {
            imageCount = surfaceDetails.SurfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = MainDevice.Surface;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = surfaceDetails.SurfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;

        if(indices.GraphicsFamily != indices.PresentationFamily)
        {
            u32 queueFamilyIndices[] = {(u32)indices.GraphicsFamily, (u32)indices.PresentationFamily};

            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        if(vkCreateSwapchainKHR(MainDevice.LogicalDevice, &swapchainCreateInfo, nullptr, &Swapchain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swap chain");
        }

        SwapchainImageFormat = surfaceFormat.format;
        SwapchainExtent = extent;

        u32 swapchainImageCount;
        vkGetSwapchainImagesKHR(MainDevice.LogicalDevice, Swapchain, &swapchainImageCount, nullptr);
        std::vector<VkImage> images(swapchainImageCount);
        vkGetSwapchainImagesKHR(MainDevice.LogicalDevice, Swapchain, &swapchainImageCount, images.data());

        for(VkImage image : images)
        {
            RenderUtil::SwapChainImage swapChainImage = {};
            swapChainImage.Image = image;
            swapChainImage.ImageView = RenderUtil::CreateImageView(image, SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, MainDevice.LogicalDevice);

            SwapchainImages.push_back(swapChainImage);
        }
    }

    VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities)
    {
        if(surfaceCapabilities.currentExtent.width != std::numeric_limits<u32>::max())
        {
            return surfaceCapabilities.currentExtent;
        }

        i32 width, height;
    #ifdef X_WINDOWING_API_GLFW
        glfwGetFramebufferSize(Window->GetWindow(), &width, &height);
    #endif
    #ifdef X_WINDOWING_API_SDL
        SDL_Vulkan_GetDrawableSize(Window::Get().GetWindow(), &width, &height);
    #endif

        VkExtent2D newExtent = {};
        newExtent.width = static_cast<u32>(width);
        newExtent.height = static_cast<u32>(height);

        newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
        newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

        return newExtent;
    }

    VkSurfaceFormatKHR SwapChain::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
    {
        if(formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }

        for(const VkSurfaceFormatKHR& format : formats)
        {
            if((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
            && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        return formats[0];
    }

    VkPresentModeKHR SwapChain::ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes)
    {
        for(const VkPresentModeKHR& presentationMode : presentationModes)
        {
            if(presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return presentationMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

}