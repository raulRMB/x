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

#include "renderutil.h"

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

    VkQueue GraphicsQueue;
    VkQueue PresentationQueue;

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
    void GetPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSurface();
    void SetupDebugMessenger();

    bool CheckInstanceExtensionSupport(std::vector<const char *> *extensions);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice Device);
    bool CheckValidationLayerSupport();
    bool CheckSuitableDevice(VkPhysicalDevice Device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator);

    xRenderUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice Device);
    xRenderUtil::SwapChainDetails GetSwapChainDetails(VkPhysicalDevice Device);
};


#endif //X_RENDERER_H
