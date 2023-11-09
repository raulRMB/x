#include <cstring>
#include <iostream>
#include <limits>
#include <array>
#include "renderer.h"
#include "../util/file.h"
#include <SDL2/SDL_vulkan.h>
#include "window.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../util/color.h"
#include "../core/Game.h"
#include "../core/Scene.h"
#include "../Components/MeshComponent.h"
#include "../Components/TransformComponent.h"
#include "../core/Camera.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>
#include "../core/SkeletalMesh.h"
#include "../Components/SkeletalMeshComponent.h"

#ifndef WIN32
#include <stdlib.h>
#endif

namespace x
{
Renderer::Renderer() :
    Instance(VK_NULL_HANDLE),
    MainDevice({VK_NULL_HANDLE, VK_NULL_HANDLE}),
    MeshList(std::vector<xMesh>()),
    ModelList(std::vector<MeshModel>()),
    Surface(VK_NULL_HANDLE),
    Swapchain(VK_NULL_HANDLE),
    SwapchainImageFormat(VK_FORMAT_UNDEFINED),
    SwapchainExtent({0, 0}),
    SwapchainFramebuffers(std::vector<VkFramebuffer>()),
    CommandBuffers(std::vector<VkCommandBuffer>()),
    RenderFinishedSemaphores(std::vector<VkSemaphore>()),
    ImageAvailableSemaphores(std::vector<VkSemaphore>()),
    TextureSampler(VK_NULL_HANDLE),
    SamplerSetLayout(VK_NULL_HANDLE),
    SamplerDescriptorPool(VK_NULL_HANDLE),
    BoneUniformAlignment(0),
    BoneTransferSpace(VK_NULL_HANDLE),
    MinUniformBufferOffset(0),
    DepthBufferImageFormat(VK_FORMAT_UNDEFINED),
    DepthBufferImage(VK_NULL_HANDLE),
    DepthBufferImageMemory(VK_NULL_HANDLE),
    DepthBufferImageView(VK_NULL_HANDLE),
    UboViewProjection({}),
    PushConstantRange({}),
    GraphicsCommandPool(VK_NULL_HANDLE),
    GraphicsQueue(VK_NULL_HANDLE),
    PresentationQueue(VK_NULL_HANDLE),
    DebugMessenger(VK_NULL_HANDLE),
    RenderPass(VK_NULL_HANDLE),
    DescriptorSetLayout(VK_NULL_HANDLE),
    DescriptorPool(VK_NULL_HANDLE),
    PipelineLayout(VK_NULL_HANDLE),
    GraphicsPipeline(VK_NULL_HANDLE),
    LinePipeline(VK_NULL_HANDLE),
    ImguiPool(VK_NULL_HANDLE)
{}

i32 Renderer::Init()
{
    try
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        GetPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateRenderPass();
        CreateDescriptorSetLayout();
        CreatePushConstantRange();
        CreatePipelineLayout();
        CreateSkeletalPipelineLayout();
        CreatePipeline(GraphicsPipeline, VK_TRUE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        CreatePipeline(LinePipeline, VK_FALSE, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
        CreateSkeletalPipeline();
        CreateDepthBufferImage();
        CreateFramebuffers();
        CreateGraphicsCommandPool();
        CreateCommandBuffers();
        CreateTextureSampler();
        AllocateDynamicBufferTransferSpace();
        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreateSynchronization();
//            CreateMeshes();
        InitImGui();
    }
    catch (const std::runtime_error& e)
    {
        printf("Error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Renderer::CreateInstance()
{
    if(EnableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "X";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "XEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> instanceExtensions = std::vector<const char*>();
    u32 extensionCount = 0;
#ifdef X_WINDOWING_API_GLFW
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    for (u32 i = 0; i < extensionCount; i++)
    {
        instanceExtensions.push_back(glfwExtensions[i]);
    }
#endif
#ifdef X_WINDOWING_API_SDL
    SDL_Vulkan_GetInstanceExtensions(Window::Get().GetWindow(), &extensionCount, nullptr);
    instanceExtensions.resize(extensionCount);
    SDL_Vulkan_GetInstanceExtensions(Window::Get().GetWindow(), &extensionCount, instanceExtensions.data());
#endif

    if(!CheckInstanceExtensionSupport(&instanceExtensions))
    {
        throw std::runtime_error("Required extensions not supported");
    }

    if (EnableValidationLayers) {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instanceCreateInfo.enabledExtensionCount = static_cast<u32>(instanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

    if (EnableValidationLayers)
    {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
    } else {
        instanceCreateInfo.enabledLayerCount = 0;
    }

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (EnableValidationLayers) {
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        instanceCreateInfo.enabledLayerCount = 0;

        instanceCreateInfo.pNext = nullptr;
    }

    #ifdef __APPLE__
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        const char** extensions = (const char **)malloc(sizeof(const char *) * 3);
        extensions[0] = "VK_KHR_surface";
        extensions[1] = "VK_EXT_metal_surface";
        extensions[2] = "VK_KHR_portability_enumeration";
        extensions[3] = "VK_EXT_debug_utils";
        instanceCreateInfo.enabledExtensionCount = 4;
        instanceCreateInfo.ppEnabledExtensionNames = extensions;
    #endif

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create Vulkan instance!");
    }
}

void Renderer::GetPhysicalDevice()
{
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

    if(deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

    for(const VkPhysicalDevice& device : devices)
    {
        if(CheckSuitableDevice(device))
        {
            MainDevice.PhysicalDevice = device;
            break;
        }
    }

    if(MainDevice.PhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable GPU");
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(MainDevice.PhysicalDevice, &deviceProperties);

    printf("Using GPU: %s\n", deviceProperties.deviceName);
    MinUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;
}

void Renderer::CreateLogicalDevice()
{
    x::RenderUtil::QueueFamilyIndices indices = GetQueueFamilies(MainDevice.PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<i32> uniqueQueueFamilies = {indices.GraphicsFamily, indices.PresentationFamily};

    for(i32 QueueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = QueueFamily;
        queueCreateInfo.queueCount = 1;
        float QueuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &QueuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = (u32)x::RenderUtil::DeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = x::RenderUtil::DeviceExtensions.data();

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if(vkCreateDevice(MainDevice.PhysicalDevice, &deviceCreateInfo, nullptr, &MainDevice.LogicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }

    vkGetDeviceQueue(MainDevice.LogicalDevice, indices.GraphicsFamily, 0, &GraphicsQueue);
    vkGetDeviceQueue(MainDevice.LogicalDevice, indices.PresentationFamily, 0, &PresentationQueue);
}

void Renderer::CreateSwapChain()
{
    x::RenderUtil::SwapChainDetails swapChainDetails = GetSwapChainDetails(MainDevice.PhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = ChooseBestSurfaceFormat(swapChainDetails.Formats);
    VkPresentModeKHR presentMode = ChooseBestPresentationMode(swapChainDetails.PresentationModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainDetails.SurfaceCapabilities);

    u32 imageCount = swapChainDetails.SurfaceCapabilities.minImageCount + 1;

    if(swapChainDetails.SurfaceCapabilities.maxImageCount > 0
        && swapChainDetails.SurfaceCapabilities.maxImageCount < imageCount)
    {
        imageCount = swapChainDetails.SurfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = Surface;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = swapChainDetails.SurfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.clipped = VK_TRUE;

    x::RenderUtil::QueueFamilyIndices indices = GetQueueFamilies(MainDevice.PhysicalDevice);

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
        x::RenderUtil::SwapChainImage swapChainImage = {};
        swapChainImage.Image = image;
        swapChainImage.ImageView = x::RenderUtil::CreateImageView(image, SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, MainDevice.LogicalDevice);

        SwapchainImages.push_back(swapChainImage);
    }
}

void Renderer::CreateSurface()
{
#ifdef X_WINDOWING_API_GLFW
        CreateSurfaceGLFW();
#endif
#ifdef X_WINDOWING_API_SDL
        CreateSurfaceSDL();
#endif
}

bool Renderer::CheckInstanceExtensionSupport(std::vector<const char*>* extensions)
{
    u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    for(const char* extension : *extensions)
    {
        bool bExtensionFound = false;
        for(const VkExtensionProperties& availableExtension : availableExtensions)
        {
            if(strcmp(extension, availableExtension.extensionName) == 0)
            {
                bExtensionFound = true;
                break;
            }
        }

        if(!bExtensionFound)
            return false;
    }

    return true;
}

bool Renderer::CheckSuitableDevice(VkPhysicalDevice device)
{
/*
    VkPhysicalDeviceProperties DeviceProperties;
    vkGetPhysicalDeviceProperties(device, &DeviceProperties);
*/
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    x::RenderUtil::QueueFamilyIndices indices = GetQueueFamilies(device);

    bool bExtensionsSupported = CheckDeviceExtensionSupport(device);

    bool bSwapChainValid;
    if(bExtensionsSupported)
    {
        x::RenderUtil::SwapChainDetails SwapChainDetails = GetSwapChainDetails(device);
        bSwapChainValid = !SwapChainDetails.Formats.empty() && !SwapChainDetails.PresentationModes.empty() && deviceFeatures.samplerAnisotropy;
    }

    return indices.IsValid() && bExtensionsSupported && bSwapChainValid;
}

x::RenderUtil::QueueFamilyIndices Renderer::GetQueueFamilies(VkPhysicalDevice device)
{
    x::RenderUtil::QueueFamilyIndices indices;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    i32 idx = 0;
    for(const VkQueueFamilyProperties& queueFamily : queueFamilies)
    {
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = idx;
        }

        VkBool32 bPresentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, Surface, &bPresentationSupport);

        if(queueFamily.queueCount > 0 && bPresentationSupport)
        {
            indices.PresentationFamily = idx;
        }

        if(indices.IsValid())
        {
            break;
        }

        idx++;
    }

    return indices;
}

x::RenderUtil::SwapChainDetails Renderer::GetSwapChainDetails(VkPhysicalDevice device)
{
    x::RenderUtil::SwapChainDetails swapChainDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Surface, &swapChainDetails.SurfaceCapabilities);

    u32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, nullptr);

    if(formatCount != 0)
    {
        swapChainDetails.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, swapChainDetails.Formats.data());
    }

    u32 presentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentationModeCount, nullptr);

    if(presentationModeCount != 0)
    {
        swapChainDetails.PresentationModes.resize(presentationModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentationModeCount, swapChainDetails.PresentationModes.data());
    }

    return swapChainDetails;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if(extensionCount == 0)
    {
        return false;
    }

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    for(const char* deviceExtension : x::RenderUtil::DeviceExtensions)
    {
        bool bExtensionFound = false;
        for(const VkExtensionProperties& availableExtension : availableExtensions)
        {
            if(strcmp(deviceExtension, availableExtension.extensionName) == 0)
            {
                bExtensionFound = true;
                break;
            }
        }

        if(!bExtensionFound)
            return false;
    }

    return true;
}

bool Renderer::CheckValidationLayerSupport()
{
    u32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : ValidationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

VkBool32 VKAPI_CALL Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                             [[maybe_unused]] void *pUserData)
{
    std::string message = "Vulkan: [TYPE:";
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    {
        message += " General";
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    {
        message += " Validation";
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        message += " Performance";
    }
    message += "] ";

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << message << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

void Renderer::SetupDebugMessenger()
{
    if(!EnableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoExt;
    PopulateDebugMessengerCreateInfo(debugUtilsMessengerCreateInfoExt);

    if (VkResult result = CreateDebugUtilsMessengerEXT(Instance, &debugUtilsMessengerCreateInfoExt, nullptr, &DebugMessenger); result != VK_SUCCESS)
    {
        printf("failed to set up debug messenger! %d", (int)result);
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult Renderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const VkAllocationCallbacks *pAllocator,
                                        VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto pFunction = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (pFunction != nullptr) {
        return pFunction(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

void Renderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkSurfaceFormatKHR Renderer::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
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

VkPresentModeKHR Renderer::ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes)
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

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities)
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

void Renderer::Clean()
{
    vkDeviceWaitIdle(MainDevice.LogicalDevice);

    vkDestroyDescriptorPool(MainDevice.LogicalDevice, ImguiPool, nullptr);
    ImGui_ImplVulkan_Shutdown();

#ifdef WIN32
    _aligned_free(BoneTransferSpace);
#else
    free(BoneTransferSpace);
#endif

    for(auto& model : ModelList)
    {
        model.DestroyMesh();
    }

    vkDestroyDescriptorPool(MainDevice.LogicalDevice, SamplerDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(MainDevice.LogicalDevice, SamplerSetLayout, nullptr);

    vkDestroySampler(MainDevice.LogicalDevice, TextureSampler, nullptr);

    for(size_t i = 0; i < TextureImages.size(); i++)
    {
        vkDestroyImageView(MainDevice.LogicalDevice, TextureImageViews[i], nullptr);
        vkDestroyImage(MainDevice.LogicalDevice, TextureImages[i], nullptr);
        vkFreeMemory(MainDevice.LogicalDevice, TextureImageMemory[i], nullptr);
    }

    vkDestroyImageView(MainDevice.LogicalDevice, DepthBufferImageView, nullptr);
    vkDestroyImage(MainDevice.LogicalDevice, DepthBufferImage, nullptr);
    vkFreeMemory(MainDevice.LogicalDevice, DepthBufferImageMemory, nullptr);

    vkDestroyDescriptorPool(MainDevice.LogicalDevice, DescriptorPool, nullptr);
    vkDestroyDescriptorPool(MainDevice.LogicalDevice, BoneDescriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(MainDevice.LogicalDevice, DescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(MainDevice.LogicalDevice, BoneDescriptorSetLayout, nullptr);

    for(size_t i = 0; i < SwapchainImages.size(); i++)
    {
        vkDestroyBuffer(MainDevice.LogicalDevice, VPUniformBuffers[i], nullptr);
        vkFreeMemory(MainDevice.LogicalDevice, VPUniformBuffersMemory[i], nullptr);
        vkDestroyBuffer(MainDevice.LogicalDevice, BoneDUniformBuffers[i], nullptr);
        vkFreeMemory(MainDevice.LogicalDevice, BoneDUniformBuffersMemory[i], nullptr);
    }

    for(xMesh& Mesh : MeshList)
    {
        Mesh.DestroyBuffers();
    }

    for(size_t i = 0; i < x::RenderUtil::MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(MainDevice.LogicalDevice, RenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(MainDevice.LogicalDevice, ImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(MainDevice.LogicalDevice, DrawFences[i], nullptr);
    }

    vkDestroyCommandPool(MainDevice.LogicalDevice, GraphicsCommandPool, nullptr);

    for(const VkFramebuffer& framebuffer : SwapchainFramebuffers)
    {
        vkDestroyFramebuffer(MainDevice.LogicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(MainDevice.LogicalDevice, GraphicsPipeline, nullptr);
    vkDestroyPipeline(MainDevice.LogicalDevice, LinePipeline, nullptr);
    vkDestroyPipelineLayout(MainDevice.LogicalDevice, PipelineLayout, nullptr);
    vkDestroyRenderPass(MainDevice.LogicalDevice, RenderPass, nullptr);

    for(const x::RenderUtil::SwapChainImage& swapChainImage : SwapchainImages)
    {
        vkDestroyImageView(MainDevice.LogicalDevice, swapChainImage.ImageView, nullptr);
    }

    vkDestroySwapchainKHR(MainDevice.LogicalDevice, Swapchain, nullptr);
    vkDestroySurfaceKHR(Instance, Surface, nullptr);

    vkDestroyDevice(MainDevice.LogicalDevice, nullptr);

    if(EnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
    }

    vkDestroyInstance(Instance, nullptr);
}

Renderer::~Renderer() = default;

void Renderer::CreateSkeletalPipeline()
{
    std::vector<char> vertShaderCode = xUtil::xFile::ReadAsBin("../assets/shaders/skeletalVert.spv");
    std::vector<char> fragShaderCode = xUtil::xFile::ReadAsBin("../assets/shaders/skeletalFrag.spv");

    VkShaderModule VertexShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule FragmentShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
    vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderCreateInfo.module = VertexShaderModule;
    vertexShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
    fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderCreateInfo.module = FragmentShaderModule;
    fragmentShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderCreateInfo, fragmentShaderCreateInfo};

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(x::RenderUtil::SkeletalVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions = {};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(x::RenderUtil::SkeletalVertex, Pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(x::RenderUtil::SkeletalVertex, Col);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(x::RenderUtil::SkeletalVertex, Tex);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SINT;
    attributeDescriptions[3].offset = offsetof(x::RenderUtil::SkeletalVertex, BoneIds);

    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[4].offset = offsetof(x::RenderUtil::SkeletalVertex, BoneWeights);

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (f32)SwapchainExtent.width;
    viewport.height = (f32)SwapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = SwapchainExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {};
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.lineWidth = 1.f;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_TRUE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
    depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = (u32)std::size(shaderStages);
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = SkeletalPipelineLayout;
    pipelineCreateInfo.renderPass = RenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(MainDevice.LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &SkeletalPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(MainDevice.LogicalDevice, VertexShaderModule, nullptr);
    vkDestroyShaderModule(MainDevice.LogicalDevice, FragmentShaderModule, nullptr);
}

void Renderer::CreatePipeline(VkPipeline& pipeline, VkBool32 depthTestEnable, VkPrimitiveTopology topology)
{
    std::vector<char> vertShaderCode = xUtil::xFile::ReadAsBin("../assets/shaders/vert.spv");
    std::vector<char> fragShaderCode = xUtil::xFile::ReadAsBin("../assets/shaders/frag.spv");

    VkShaderModule VertexShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule FragmentShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
    vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderCreateInfo.module = VertexShaderModule;
    vertexShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
    fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderCreateInfo.module = FragmentShaderModule;
    fragmentShaderCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderCreateInfo, fragmentShaderCreateInfo};

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(x::RenderUtil::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(x::RenderUtil::Vertex, Pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(x::RenderUtil::Vertex, Col);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(x::RenderUtil::Vertex, Tex);

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
    vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = topology;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

//    std::vector<VkDynamicState> dynamicStateEnables;
//    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
//    dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

//    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
//    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//    dynamicStateCreateInfo.dynamicStateCount = (u32)dynamicStateEnables.size();
//    dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (f32)SwapchainExtent.width;
    viewport.height = (f32)SwapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = SwapchainExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {};
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.lineWidth = 1.f;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_TRUE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
    colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendCreateInfo.attachmentCount = 1;
    colorBlendCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendCreateInfo.blendConstants[0] = 0.0f;
    colorBlendCreateInfo.blendConstants[1] = 0.0f;
    colorBlendCreateInfo.blendConstants[2] = 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
    depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilCreateInfo.depthTestEnable = depthTestEnable;
    depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = (u32)std::size(shaderStages);
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = PipelineLayout;
    pipelineCreateInfo.renderPass = RenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(MainDevice.LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(MainDevice.LogicalDevice, VertexShaderModule, nullptr);
    vkDestroyShaderModule(MainDevice.LogicalDevice, FragmentShaderModule, nullptr);
}

VkShaderModule Renderer::CreateShaderModule(const std::vector<char>& bytes) const
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = bytes.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const u32*>(bytes.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(MainDevice.LogicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }

    return shaderModule;
}

void Renderer::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = SwapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    DepthBufferImageFormat = ChooseSupportedFormat(
            {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = DepthBufferImageFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

    std::array<VkSubpassDependency, 2> subpassDependencies = std::array<VkSubpassDependency, 2>();

    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = 0;

    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags = 0;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = (u32)attachments.size();
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = (u32)subpassDependencies.size();
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    if(vkCreateRenderPass(MainDevice.LogicalDevice, &renderPassCreateInfo, nullptr, &RenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass");
    }
}

void Renderer::CreateFramebuffers()
{
    SwapchainFramebuffers.resize(SwapchainImages.size());

    for(size_t i = 0; i < SwapchainFramebuffers.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
                SwapchainImages[i].ImageView,
                DepthBufferImageView
        };

        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = RenderPass;
        framebufferCreateInfo.attachmentCount = (u32)attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = SwapchainExtent.width;
        framebufferCreateInfo.height = SwapchainExtent.height;
        framebufferCreateInfo.layers = 1;

        if(vkCreateFramebuffer(MainDevice.LogicalDevice, &framebufferCreateInfo, nullptr, &SwapchainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void Renderer::CreateGraphicsCommandPool()
{
    x::RenderUtil::QueueFamilyIndices queueFamilyIndices = GetQueueFamilies(MainDevice.PhysicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily;

    if(vkCreateCommandPool(MainDevice.LogicalDevice, &commandPoolCreateInfo, nullptr, &GraphicsCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics command pool");
    }
}

void Renderer::CreateCommandBuffers()
{
    CommandBuffers.resize(SwapchainFramebuffers.size());

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = GraphicsCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = (u32)CommandBuffers.size();

    if(vkAllocateCommandBuffers(MainDevice.LogicalDevice, &commandBufferAllocateInfo, CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffer s");
    }
}

void Renderer::RecordCommands(u32 currentImage)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = RenderPass;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = SwapchainExtent;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil.depth = 1.0f;

    renderPassBeginInfo.clearValueCount = (u32)clearValues.size();
    renderPassBeginInfo.pClearValues = clearValues.data();

    renderPassBeginInfo.framebuffer = SwapchainFramebuffers[currentImage];

    if(vkBeginCommandBuffer(CommandBuffers[currentImage], &commandBufferBeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin recording command buffer");
    }
        vkCmdBeginRenderPass(CommandBuffers[currentImage], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(CommandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

                auto view = Game::GetInstance().GetScene()->GetRegistry().view<CTransform3d, CTriangleMesh>();
                for(entt::entity entity : view)
                {
                    MeshModel& model = ModelList[view.get<CTriangleMesh>(entity).Id];
                    vkCmdPushConstants(CommandBuffers[currentImage], PipelineLayout,
                                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xModel),
                                       &model.GetModel());

                    for(size_t i = 0; i < model.GetMeshCount(); i++)
                    {
                        const xMesh* mesh = model.GetMesh(i);

                        VkBuffer vertexBuffers[] = {mesh->GetVertexBuffer()};
                        VkDeviceSize offsets[] = {0};
                        vkCmdBindVertexBuffers(CommandBuffers[currentImage], 0, 1, vertexBuffers, offsets);
                        vkCmdBindIndexBuffer(CommandBuffers[currentImage], mesh->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

                        //                u32 dynamicOffset = (u32)ModelUniformAlignment * j;

                        std::array<VkDescriptorSet, 2> descriptorSets =
                        {
                            DescriptorSets[currentImage],
                            SamplerDescriptorSets[mesh->GetTextureId()]
                        };

                        vkCmdBindDescriptorSets(CommandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                PipelineLayout, 0, (u32)descriptorSets.size(), descriptorSets.data(),
                                                0, nullptr);

                        vkCmdDrawIndexed(CommandBuffers[currentImage], mesh->GetIndexCount(), 1, 0, 0, 0);
                    }
                }

            vkCmdBindPipeline(CommandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, LinePipeline);

                auto lineView = Game::GetInstance().GetScene()->GetRegistry().view<CTransform3d, CLineMesh>();
                for(entt::entity entity : lineView)
                {
                    CTransform3d transform3d = lineView.get<CTransform3d>(entity);
                    xMesh& mesh = MeshList[lineView.get<CLineMesh>(entity).Id];
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::scale(glm::mat4(1.0f), transform3d.WorldScale) * model;
                    model = glm::rotate(glm::mat4(1.0f), transform3d.WorldRotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * model;
                    model = glm::rotate(glm::mat4(1.0f), transform3d.WorldRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * model;
                    model = glm::rotate(glm::mat4(1.0f), transform3d.WorldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) * model;
                    model = glm::translate(glm::mat4(1.0f), transform3d.WorldPosition) * model;
                    vkCmdPushConstants(CommandBuffers[currentImage], PipelineLayout,
                                   VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xModel),
                                   &model);

                    VkBuffer vertexBuffers[] = {mesh.GetVertexBuffer()};
                    VkDeviceSize offsets[] = {0};
                    vkCmdBindVertexBuffers(CommandBuffers[currentImage], 0, 1, vertexBuffers, offsets);
                    vkCmdBindIndexBuffer(CommandBuffers[currentImage], mesh.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

                    //                u32 dynamicOffset = (u32)ModelUniformAlignment * j;

                    std::array<VkDescriptorSet, 2> descriptorSets =
                    {
                            DescriptorSets[currentImage],
                            SamplerDescriptorSets[mesh.GetTextureId()]
                    };

                    vkCmdBindDescriptorSets(CommandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            PipelineLayout, 0, (u32)descriptorSets.size(), descriptorSets.data(),
                                            0, nullptr);

                    vkCmdDrawIndexed(CommandBuffers[currentImage], mesh.GetIndexCount(), 1, 0, 0, 0);
                }

            vkCmdBindPipeline(CommandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, SkeletalPipeline);

                auto skeletalView = Game::GetInstance().GetScene()->GetRegistry().view<CTransform3d, CSkeletalMesh>();
                for(entt::entity entity : skeletalView)
                {
                    CTransform3d transform3d = skeletalView.get<CTransform3d>(entity);
                    SkeletalMesh& mesh = SkeletalMeshList[skeletalView.get<CSkeletalMesh>(entity).Id];
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::scale(glm::mat4(1.0f), transform3d.WorldScale) * model;
                    model = glm::rotate(glm::mat4(1.0f), transform3d.WorldRotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * model;
                    model = glm::rotate(glm::mat4(1.0f), transform3d.WorldRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * model;
                    model = glm::rotate(glm::mat4(1.0f), transform3d.WorldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) * model;
                    model = glm::translate(glm::mat4(1.0f), transform3d.WorldPosition) * model;
                    vkCmdPushConstants(CommandBuffers[currentImage], SkeletalPipelineLayout,
                                       VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xModel),
                                       &model);

                    VkBuffer vertexBuffers[] = {mesh.GetVertexBuffer()};
                    VkDeviceSize offsets[] = {0};
                    vkCmdBindVertexBuffers(CommandBuffers[currentImage], 0, 1, vertexBuffers, offsets);
                    vkCmdBindIndexBuffer(CommandBuffers[currentImage], mesh.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

                    std::array<VkDescriptorSet, 3> descriptorSets =
                    {
                        DescriptorSets[currentImage],
                        SamplerDescriptorSets[mesh.GetTextureId()],
                        BoneDescriptorSets[currentImage]
                    };

                    u32 dynamicOffset = 0;
                    vkCmdBindDescriptorSets(CommandBuffers[currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            SkeletalPipelineLayout, 0, (u32)descriptorSets.size(), descriptorSets.data(),
                                            1, &dynamicOffset);

                    vkCmdDrawIndexed(CommandBuffers[currentImage], mesh.GetIndexCount(), 1, 0, 0, 0);
                }


            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CommandBuffers[currentImage]);

        vkCmdEndRenderPass(CommandBuffers[currentImage]);

    if(vkEndCommandBuffer(CommandBuffers[currentImage]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to record command buffer");
    }
}

void Renderer::DrawFrame()
{
    vkWaitForFences(MainDevice.LogicalDevice, 1, &DrawFences[CurrentFrame], VK_TRUE, std::numeric_limits<u64>::max());
    vkResetFences(MainDevice.LogicalDevice, 1, &DrawFences[CurrentFrame]);

    u32 imageIndex;
    vkAcquireNextImageKHR(MainDevice.LogicalDevice, Swapchain, std::numeric_limits<u64>::max(), ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);

    Scene* scene = Game::GetInstance().GetScene();
    scene->GetRegistry().view<CTransform3d, CTriangleMesh>().each([&](CTransform3d& t, CTriangleMesh& m)
    {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::scale(glm::mat4(1.0f), t.WorldScale) * transform;
        transform = glm::rotate(glm::mat4(1.0f), t.WorldRotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * transform;
        transform = glm::rotate(glm::mat4(1.0f), t.WorldRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * transform;
        transform = glm::rotate(glm::mat4(1.0f), t.WorldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) * transform;
        transform = glm::translate(glm::mat4(1.0f), t.WorldPosition) * transform;
        UpdateModel(m.Id, transform);
    });


    RecordCommands(imageIndex);
    UpdateUniformBuffers(imageIndex);

    UboViewProjection.View = CameraSystem::Get().GetMainCameraView();
    UboViewProjection.Projection = CameraSystem::Get().GetMainCameraProjection();

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {ImageAvailableSemaphores[CurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = (u32)std::size(waitSemaphores);
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {RenderFinishedSemaphores[CurrentFrame]};
    submitInfo.signalSemaphoreCount = (u32)std::size(signalSemaphores);
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(GraphicsQueue, 1, &submitInfo, DrawFences[CurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = (u32)std::size(signalSemaphores);
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = {Swapchain};
    presentInfo.swapchainCount = (u32)std::size(swapchains);
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    if(vkQueuePresentKHR(PresentationQueue, &presentInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present image");
    }

    CurrentFrame = (CurrentFrame + 1) % x::RenderUtil::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::CreateSynchronization()
{
    ImageAvailableSemaphores.resize(x::RenderUtil::MAX_FRAMES_IN_FLIGHT);
    RenderFinishedSemaphores.resize(x::RenderUtil::MAX_FRAMES_IN_FLIGHT);
    DrawFences.resize(x::RenderUtil::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < x::RenderUtil::MAX_FRAMES_IN_FLIGHT; i++)
    {
        if(vkCreateSemaphore(MainDevice.LogicalDevice, &semaphoreCreateInfo, nullptr, &ImageAvailableSemaphores[i]) != VK_SUCCESS
           || vkCreateSemaphore(MainDevice.LogicalDevice, &semaphoreCreateInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS
            || vkCreateFence(MainDevice.LogicalDevice, &fenceCreateInfo, nullptr, &DrawFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create synchronization objects for a frame");
        }
    }
}

void Renderer::CreateSurfaceGLFW()
{
#ifdef X_WINDOWING_API_GLFW
    if(glfwCreateWindowSurface(Instance, Window->GetWindow(), nullptr, &Surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create surface");
    }
#endif
}

void Renderer::CreateSurfaceSDL()
{
#ifdef X_WINDOWING_API_SDL
    if(SDL_Vulkan_CreateSurface(Window::Get().GetWindow(), Instance, &Surface) != SDL_TRUE)
    {
        throw std::runtime_error("Failed to create surface");
    }
#endif
}

void Renderer::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding vpLayoutBinding{};
    vpLayoutBinding.binding = 0;
    vpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vpLayoutBinding.descriptorCount = 1;
    vpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vpLayoutBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {vpLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = (u32)layoutBindings.size();
    layoutCreateInfo.pBindings = layoutBindings.data();

    if(vkCreateDescriptorSetLayout(MainDevice.LogicalDevice, &layoutCreateInfo, nullptr, &DescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout");
    }

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo samplerLayoutCreateInfo{};
    samplerLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    samplerLayoutCreateInfo.bindingCount = 1;
    samplerLayoutCreateInfo.pBindings = &samplerLayoutBinding;

    if(vkCreateDescriptorSetLayout(MainDevice.LogicalDevice, &samplerLayoutCreateInfo, nullptr, &SamplerSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create sampler descriptor set layout");
    }

    VkDescriptorSetLayoutBinding boneLayoutBinding{};
    boneLayoutBinding.binding = 1;
    boneLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    boneLayoutBinding.descriptorCount = 1;
    boneLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    boneLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo boneLayoutCreateInfo{};
    boneLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutBindings.push_back(boneLayoutBinding);
    boneLayoutCreateInfo.bindingCount = (u32)layoutBindings.size();
    boneLayoutCreateInfo.pBindings = layoutBindings.data();

    if(vkCreateDescriptorSetLayout(MainDevice.LogicalDevice, &boneLayoutCreateInfo, nullptr, &BoneDescriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create bone descriptor set layout");
    }
}

void Renderer::CreateUniformBuffers()
{
    VkDeviceSize vpBufferSize = sizeof(UBOViewProjection);

    VPUniformBuffers.resize(SwapchainImages.size());
    VPUniformBuffersMemory.resize(SwapchainImages.size());

    VkDeviceSize boneBufferSize = BoneUniformAlignment * x::RenderUtil::MAX_OBJECTS;

    BoneDUniformBuffers.resize(SwapchainImages.size());
    BoneDUniformBuffersMemory.resize(SwapchainImages.size());

    for(size_t i = 0; i < SwapchainImages.size(); i++)
    {
        x::RenderUtil::CreateBuffer(MainDevice.PhysicalDevice, MainDevice.LogicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             VPUniformBuffers[i], VPUniformBuffersMemory[i]);

        x::RenderUtil::CreateBuffer(MainDevice.PhysicalDevice, MainDevice.LogicalDevice, boneBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    BoneDUniformBuffers[i], BoneDUniformBuffersMemory[i]);
    }
}

void Renderer::CreateDescriptorPool()
{
    VkDescriptorPoolSize vpPoolSize{};
    vpPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vpPoolSize.descriptorCount = (u32)VPUniformBuffers.size();

    std::vector<VkDescriptorPoolSize> poolSizes = {vpPoolSize /*, modelPoolSize*/};

    VkDescriptorPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.maxSets = (u32)SwapchainImages.size();
    poolCreateInfo.poolSizeCount = (u32)poolSizes.size();
    poolCreateInfo.pPoolSizes = poolSizes.data();

    if(vkCreateDescriptorPool(MainDevice.LogicalDevice, &poolCreateInfo, nullptr, &DescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    VkDescriptorPoolSize samplerPoolSize{};
    samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerPoolSize.descriptorCount = x::RenderUtil::MAX_OBJECTS;

    VkDescriptorPoolCreateInfo samplerPoolCreateInfo{};
    samplerPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    samplerPoolCreateInfo.maxSets = x::RenderUtil::MAX_OBJECTS;
    samplerPoolCreateInfo.poolSizeCount = 1;
    samplerPoolCreateInfo.pPoolSizes = &samplerPoolSize;

    if(vkCreateDescriptorPool(MainDevice.LogicalDevice, &samplerPoolCreateInfo, nullptr, &SamplerDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create sampler descriptor pool");
    }

    VkDescriptorPoolSize bonePoolSize{};
    bonePoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    bonePoolSize.descriptorCount = (u32)BoneDUniformBuffers.size();

    VkDescriptorPoolCreateInfo bonePoolCreateInfo{};
    bonePoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    bonePoolCreateInfo.maxSets = x::RenderUtil::MAX_OBJECTS;
    bonePoolCreateInfo.poolSizeCount = 1;
    bonePoolCreateInfo.pPoolSizes = &bonePoolSize;

    if(vkCreateDescriptorPool(MainDevice.LogicalDevice, &bonePoolCreateInfo, nullptr, &BoneDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create bone descriptor pool");
    }
}

void Renderer::CreateDescriptorSets()
{
    DescriptorSets.resize(SwapchainImages.size());

    std::vector<VkDescriptorSetLayout> setLayouts(SwapchainImages.size(), DescriptorSetLayout);

    VkDescriptorSetAllocateInfo setAllocateInfo{};
    setAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocateInfo.descriptorPool = DescriptorPool;
    setAllocateInfo.descriptorSetCount = (u32)SwapchainImages.size();
    setAllocateInfo.pSetLayouts = setLayouts.data();

    if(vkAllocateDescriptorSets(MainDevice.LogicalDevice, &setAllocateInfo, DescriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }

    BoneDescriptorSets.resize(SwapchainImages.size());

    std::vector<VkDescriptorSetLayout> boneSetLayouts(SwapchainImages.size(), BoneDescriptorSetLayout);

    VkDescriptorSetAllocateInfo boneSetAllocateInfo{};
    boneSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    boneSetAllocateInfo.descriptorPool = BoneDescriptorPool;
    boneSetAllocateInfo.descriptorSetCount = (u32)SwapchainImages.size();
    boneSetAllocateInfo.pSetLayouts = boneSetLayouts.data();

    if(vkAllocateDescriptorSets(MainDevice.LogicalDevice, &boneSetAllocateInfo, BoneDescriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate bone descriptor sets");
    }

    for(size_t i = 0; i < SwapchainImages.size(); i++)
    {
        VkDescriptorBufferInfo vpBufferInfo{};
        vpBufferInfo.buffer = VPUniformBuffers[i];
        vpBufferInfo.offset = 0;
        vpBufferInfo.range = sizeof(UBOViewProjection);

        VkWriteDescriptorSet vpSetWrite{};
        vpSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        vpSetWrite.dstSet = DescriptorSets[i];
        vpSetWrite.dstBinding = 0;
        vpSetWrite.dstArrayElement = 0;
        vpSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        vpSetWrite.descriptorCount = 1;
        vpSetWrite.pBufferInfo = &vpBufferInfo;

        VkDescriptorBufferInfo boneBufferInfo{};
        boneBufferInfo.buffer = BoneDUniformBuffers[i];
        boneBufferInfo.offset = 0;
        boneBufferInfo.range = BoneUniformAlignment;

        VkWriteDescriptorSet boneSetWrite{};
        boneSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        boneSetWrite.dstSet = BoneDescriptorSets[i];
        boneSetWrite.dstBinding = 1;
        boneSetWrite.dstArrayElement = 0;
        boneSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        boneSetWrite.descriptorCount = 1;
        boneSetWrite.pBufferInfo = &boneBufferInfo;

        std::vector<VkWriteDescriptorSet> setWrites = { vpSetWrite , boneSetWrite };
        vkUpdateDescriptorSets(MainDevice.LogicalDevice, setWrites.size(), setWrites.data(), 0, nullptr);
    }
}

void Renderer::UpdateUniformBuffers(u32 imageIndex)
{
    void* data;
    vkMapMemory(MainDevice.LogicalDevice, VPUniformBuffersMemory[imageIndex], 0, sizeof(UBOViewProjection), 0, &data);
    memcpy(data, &UboViewProjection, sizeof(UBOViewProjection));
    vkUnmapMemory(MainDevice.LogicalDevice, VPUniformBuffersMemory[imageIndex]);

    /*  DYNAMIC UNIFORM BUFFER TEST */
    for(size_t i = 0; i < SkeletalMeshList.size(); i++)
    {
        BoneTransforms* bones = (BoneTransforms*)((u64)BoneTransferSpace + (i * BoneUniformAlignment));
        for(size_t j = 0; j < SkeletalMeshList[i].GetBoneCount(); j++)
        {
            bones->Bones[j] = SkeletalMeshList[i].GetCurrentPose()[j];
        }
    }

    vkMapMemory(MainDevice.LogicalDevice, BoneDUniformBuffersMemory[imageIndex], 0, BoneUniformAlignment * SkeletalMeshList.size(), 0, &data);
    memcpy(data, BoneTransferSpace, BoneUniformAlignment * SkeletalMeshList.size());
    vkUnmapMemory(MainDevice.LogicalDevice, BoneDUniformBuffersMemory[imageIndex]);
}

void Renderer::UpdateModel(u32 modelId, glm::mat4 newModel)
{
    if(modelId < ModelList.size())
    {
        ModelList[modelId].SetModel(newModel);
    }
}

void Renderer::CreatePushConstantRange()
{
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(xModel);
}

void Renderer::CreateDepthBufferImage()
{
    DepthBufferImage = x::RenderUtil::CreateImage(SwapchainExtent.width, SwapchainExtent.height,
                                           DepthBufferImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, DepthBufferImageMemory, MainDevice.LogicalDevice, MainDevice.PhysicalDevice);

    DepthBufferImageView = x::RenderUtil::CreateImageView(DepthBufferImage, DepthBufferImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, MainDevice.LogicalDevice);
}

VkFormat Renderer::ChooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling,
                                          VkFormatFeatureFlags featureFlags)
{
    for(VkFormat format : formats)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(MainDevice.PhysicalDevice, format, &properties);

        if((tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags) ||
            (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags))
        {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format");
}

i32 Renderer::CreateTextureImage(const std::string& fileName)
{
    i32 width, height;
    VkDeviceSize imageSize;
    stbi_uc* imageData = x::RenderUtil::LoadTextureFile(fileName, &width, &height, &imageSize);

    VkBuffer imageStagingBuffer;
    VkDeviceMemory imageStagingBufferMemory;
    x::RenderUtil::CreateBuffer(MainDevice.PhysicalDevice, MainDevice.LogicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 imageStagingBuffer, imageStagingBufferMemory);

    void* data;
    vkMapMemory(MainDevice.LogicalDevice, imageStagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, imageData, static_cast<size_t>(imageSize));
    vkUnmapMemory(MainDevice.LogicalDevice, imageStagingBufferMemory);
    stbi_image_free(imageData);

    VkImage texImage;
    VkDeviceMemory texImageMemory;
    texImage = x::RenderUtil::CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texImageMemory, MainDevice.LogicalDevice, MainDevice.PhysicalDevice);

    x::RenderUtil::TransitionImageLayout(MainDevice.LogicalDevice, GraphicsQueue, GraphicsCommandPool, texImage, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    x::RenderUtil::CopyImageBuffer(MainDevice.LogicalDevice, GraphicsQueue, GraphicsCommandPool,
                    imageStagingBuffer, texImage, width, height);

    x::RenderUtil::TransitionImageLayout(MainDevice.LogicalDevice, GraphicsQueue, GraphicsCommandPool, texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    TextureImages.push_back(texImage);
    TextureImageMemory.push_back(texImageMemory);

    vkDestroyBuffer(MainDevice.LogicalDevice, imageStagingBuffer, nullptr);
    vkFreeMemory(MainDevice.LogicalDevice, imageStagingBufferMemory, nullptr);

    return (i32)TextureImages.size() - 1;
}

i32 Renderer::CreateTexture(const std::string &fileName)
{
    i32 textureImageLoc = CreateTextureImage(fileName);

    VkImageView imageView = x::RenderUtil::CreateImageView(TextureImages[textureImageLoc],
                                                    VK_FORMAT_R8G8B8A8_UNORM,
                                                    VK_IMAGE_ASPECT_COLOR_BIT,
                                                    MainDevice.LogicalDevice);

    TextureImageViews.push_back(imageView);

    i32 descriptorLoc = CreateTextureDescriptor(imageView);

    return descriptorLoc;
}

void Renderer::CreateTextureSampler()
{
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR; // How to render when image is magnified on screen
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR; // How to render when image is minified on screen
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // How to handle texture wrap in U (x) direction
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // How to handle texture wrap in V (y) direction
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // How to handle texture wrap in W (z) direction
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // Border beyond texture (only works for border clamp)
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE; // Whether coords should be normalized between 0 and 1
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // Mipmap interpolation mode
    samplerCreateInfo.mipLodBias = 0.0f; // Level of detail bias for mip level
    samplerCreateInfo.minLod = 0.0f; // Minimum level of detail to pick mip level
    samplerCreateInfo.maxLod = 0.0f; // Maximum level of detail to pick mip level
    samplerCreateInfo.anisotropyEnable = VK_TRUE; // Enable anisotropy
    samplerCreateInfo.maxAnisotropy = 16; // Anisotropy sample level

    if(vkCreateSampler(MainDevice.LogicalDevice, &samplerCreateInfo, nullptr, &TextureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

i32 Renderer::CreateTextureDescriptor(VkImageView textureImage)
{
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo setAllocateInfo{};
    setAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocateInfo.descriptorPool = SamplerDescriptorPool;
    setAllocateInfo.descriptorSetCount = 1;
    setAllocateInfo.pSetLayouts = &SamplerSetLayout;

    if(vkAllocateDescriptorSets(MainDevice.LogicalDevice, &setAllocateInfo, &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate texture descriptor set");
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImage;
    imageInfo.sampler = TextureSampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(MainDevice.LogicalDevice, 1, &descriptorWrite, 0, nullptr);

    SamplerDescriptorSets.push_back(descriptorSet);

    return (i32)SamplerDescriptorSets.size() - 1;
}

void Renderer::CreateMesh(const std::string& texture, x::Primitives2D::Shape shape, const glm::vec4& color)
{
    std::vector<x::RenderUtil::Vertex> Verts;
    std::vector<u32> Indices;
    switch(shape)
    {
        case x::Primitives2D::Shape::Square:
            Verts = x::Primitives2D::MakeSquare(color);
            Indices = { 0, 1, 2, 3, 0 };
            break;
        case x::Primitives2D::Shape::Triangle:
            break;
        case x::Primitives2D::Shape::Circle:
            Verts = x::Primitives2D::MakeCircle(color, 20);
            Indices = std::vector<u32>();
            Indices.reserve(21);
            for (u32 i = 0; i < 20; i++)
            {
                Indices.push_back(i);
            }
            Indices.push_back(0);
            break;
        default:
            break;
    }

    auto TextureId = CreateTexture(texture);

    MeshList.emplace_back(Verts, Indices, TextureId,
                          GraphicsQueue, GraphicsCommandPool,
                          MainDevice.PhysicalDevice, MainDevice.LogicalDevice);
}

glm::mat4 Renderer::GetViewProjectionInverse()
{
    return glm::inverse(UboViewProjection.View * UboViewProjection.Projection);
}

u32 Renderer::CreateMeshModel(const std::string &fileName)
{
    Assimp::Importer importer;
    u32 flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
    const aiScene* scene = importer.ReadFile(fileName, flags);
    if(!scene)
    {
        throw std::runtime_error("Failed to load model" + fileName + ")");
    }
    std::vector<std::string> textureNames = MeshModel::LoadMaterials(scene);

    std::vector<i32> matToTex(textureNames.size());
    for(size_t i = 0; i < textureNames.size(); i++)
    {
        if(textureNames[i].empty())
        {
            matToTex[i] = 0;
        }
        else
        {
            matToTex[i] = CreateTexture(textureNames[i]);
        }
    }

    std::vector<xMesh> modelMeshes = MeshModel::LoadNode(MainDevice.PhysicalDevice, MainDevice.LogicalDevice,
                                                         GraphicsQueue, GraphicsCommandPool, scene->mRootNode,
                                                         scene, matToTex);

    MeshModel meshModel = MeshModel(modelMeshes);
    ModelList.push_back(meshModel);

    return (u32)ModelList.size() - 1;
}

Renderer &Renderer::Get()
{
    static Renderer instance;
    return instance;
}

void Renderer::InitImGui()
{
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if(vkCreateDescriptorPool(MainDevice.LogicalDevice, &pool_info, nullptr, &ImguiPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create imgui descriptor pool");
    }

    // 2: initialize imgui library

    //this initializes the core structures of imgui
    ImGui::CreateContext();

    //this initializes imgui for SDL
    ImGui_ImplSDL2_InitForVulkan(Window::Get().GetWindow());

    //this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = Instance;
    init_info.PhysicalDevice = MainDevice.PhysicalDevice;
    init_info.Device = MainDevice.LogicalDevice;
    init_info.Queue = GraphicsQueue;
    init_info.DescriptorPool = ImguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, RenderPass);

    {
        VkCommandPool command_pool = GraphicsCommandPool;
        VkCommandBuffer command_buffer = CommandBuffers[0];

        if(vkResetCommandPool(MainDevice.LogicalDevice, command_pool, 0) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to reset command pool");
        }

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin command buffer");
        }

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to end command buffer");
        }
        if(vkQueueSubmit(GraphicsQueue, 1, &end_info, VK_NULL_HANDLE) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit queue");
        }

        if(vkDeviceWaitIdle(MainDevice.LogicalDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to wait for device");
        }
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

u32 Renderer::CreateLine(const v3 &start, const v3 &end, const v4 &color)
{
    std::vector<x::RenderUtil::Vertex> verts= {
        {start, color},
        {end, color}
    };

    std::vector<u32> indices = {0, 1};

    MeshList.emplace(MeshList.end(), verts, indices, 0,
                     GraphicsQueue, GraphicsCommandPool,
                     MainDevice.PhysicalDevice, MainDevice.LogicalDevice);

    return (u32)MeshList.size() - 1;
}

void Renderer::CreatePipelineLayout()
{
    std::array<VkDescriptorSetLayout, 2> setLayouts =
    {
        DescriptorSetLayout,
        SamplerSetLayout
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = (u32)setLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &PushConstantRange;

    if(vkCreatePipelineLayout(MainDevice.LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void Renderer::CreateSkeletalPipelineLayout()
{
    std::array<VkDescriptorSetLayout, 3> setLayouts =
    {
        DescriptorSetLayout,
        SamplerSetLayout,
        BoneDescriptorSetLayout
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = (u32)setLayouts.size();
    pipelineLayoutCreateInfo.pSetLayouts = setLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &PushConstantRange;

    if(vkCreatePipelineLayout(MainDevice.LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &SkeletalPipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

u32 Renderer::CreateTriangle(v2 vec1, v2 vec2, v2 vec3, glm::vec4 vec4)
{
    std::vector<x::RenderUtil::Vertex> verts = {
            {v3(vec1.x, 0.f, vec1.y), vec4},
            {v3(vec2.x, 0.f, vec2.y), vec4},
            {v3(vec3.x, 0.f, vec3.y), vec4}
    };

    std::vector<u32> indices = {0, 1, 2, 0};

    MeshList.emplace(MeshList.end(), verts, indices, 0,
                     GraphicsQueue, GraphicsCommandPool,
                     MainDevice.PhysicalDevice, MainDevice.LogicalDevice);

    return (u32)MeshList.size() - 1;
}

void Renderer::CreateSkeletalMesh(const std::string &fileName)
{
    Assimp::Importer importer;
    u32 flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
    const aiScene* scene = importer.ReadFile(fileName, flags);
    if(!scene)
    {
        throw std::runtime_error("Failed to load model " + fileName + ")");
    }
    std::vector<std::string> textureNames = MeshModel::LoadMaterials(scene);

    std::vector<i32> matToTex(textureNames.size());
    for(size_t i = 0; i < textureNames.size(); i++)
    {
        if(textureNames[i].empty())
        {
            matToTex[i] = 0;
        }
        else
        {
            matToTex[i] = CreateTexture(textureNames[i]);
        }
    }

    SkeletalMesh mesh = SkeletalMesh::LoadMesh(MainDevice.PhysicalDevice, MainDevice.LogicalDevice,
                                                         GraphicsQueue, GraphicsCommandPool, scene,
                                                         scene->mMeshes[0], matToTex[0]);

    SkeletalMeshList.emplace_back(mesh);
}

SkeletalMesh& Renderer::GetSkeletalMesh(u32 id)
{
    return SkeletalMeshList[id];
}

u32 findClosestPowerOf2(u32 x)
{
    u32 result = 1;
    while (result < x)
    {
        result <<= 1;
    }
    return result;
}

void Renderer::AllocateDynamicBufferTransferSpace()
{
    u32 size = (u32)sizeof(BoneTransforms);
    BoneUniformAlignment = (size + MinUniformBufferOffset - 1) & ~(MinUniformBufferOffset - 1);
#ifdef WIN32
    BoneTransferSpace = (BoneTransforms*)_aligned_malloc(BoneUniformAlignment, findClosestPowerOf2(BoneUniformAlignment));
#else
    BoneTransferSpace = (BoneTransforms*)aligned_alloc(BoneUniformAlignment, findClosestPowerOf2(BoneUniformAlignment));
#endif
}
}
