//
// Created by Raul Romero on 2023-09-22.
//

#ifndef X_RENDERER_H
#define X_RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "base/defines.h"

#include <stdexcept>
#include <vector>
#include <set>

#include "rutil.h"

class xRenderer {
private:
    friend class xEngine;

    GLFWwindow* Window;

    VkInstance Instance;

    struct
    {
        VkPhysicalDevice PhysicalDevice;
        VkDevice LogicalDevice;
    } MainDevice;

    VkSurfaceKHR Surface;

    VkSwapchainKHR Swapchain;
    std::vector<xRUtil::SwapChainImage> SwapchainImages;
    VkFormat SwapchainImageFormat;
    VkExtent2D SwapchainExtent;

    VkQueue GraphicsQueue;
    VkQueue PresentationQueue;

    VkPipelineLayout PipelineLayout;
    VkRenderPass RenderPass;
    VkPipeline GraphicsPipeline;

    const std::vector<const char*> ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool EnableValidationLayers = false;
#else
    const bool EnableValidationLayers = true;
#endif

    VkDebugUtilsMessengerEXT DebugMessenger;

private:
    xRenderer();
    ~xRenderer();

    i32 Init(GLFWwindow* window);

    void Clean();

private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void GetPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapChain();
    void CreateGraphicsPipeline();
    void CreateRenderPass();

    static bool CheckInstanceExtensionSupport(std::vector<const char *> *extensions);
    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    bool CheckValidationLayerSupport();
    bool CheckSuitableDevice(VkPhysicalDevice device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            [[maybe_unused]] void* pUserData);

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    static VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);

    static void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator);

    xRUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);
    xRUtil::SwapChainDetails GetSwapChainDetails(VkPhysicalDevice device);

    static VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    static VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectFlags) const;

    VkShaderModule CreateShaderModule(const std::vector<byte>& bytes) const;
};


#endif //X_RENDERER_H
