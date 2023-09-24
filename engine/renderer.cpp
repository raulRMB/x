//
// Created by Raul Romero on 2023-09-22.
//

#include <cstring>
#include <iostream>
#include "renderer.h"

xRenderer::xRenderer() :
        Window(nullptr),
        Instance(VK_NULL_HANDLE),
        MainDevice({VK_NULL_HANDLE, VK_NULL_HANDLE}),
        GraphicsQueue(VK_NULL_HANDLE),
        PresentationQueue(VK_NULL_HANDLE),
        DebugMessenger(VK_NULL_HANDLE),
        Surface(VK_NULL_HANDLE)
{}

i32 xRenderer::Init(GLFWwindow *window)
{
    Window = window;

    try
    {
        CreateInstance();
        CreateSurface();
        SetupDebugMessenger();
        GetPhysicalDevice();
        CreateLogicalDevice();
    } catch (const std::runtime_error& e)
    {
        printf("Error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void xRenderer::CreateInstance()
{
    if(EnableValidationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "X";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "XEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> instanceExtensions = std::vector<const char*>();
    u32 glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (u32 i = 0; i < glfwExtensionCount; i++)
    {
        instanceExtensions.push_back(glfwExtensions[i]);
    }

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

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create Vulkan instance!");
    }
}

void xRenderer::GetPhysicalDevice()
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
}

void xRenderer::CreateLogicalDevice()
{
    xRenderUtil::QueueFamilyIndices indices = GetQueueFamilies(MainDevice.PhysicalDevice);

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
    deviceCreateInfo.enabledExtensionCount = (u32)xRenderUtil::DeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = xRenderUtil::DeviceExtensions.data();

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if(vkCreateDevice(MainDevice.PhysicalDevice, &deviceCreateInfo, nullptr, &MainDevice.LogicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }

    vkGetDeviceQueue(MainDevice.LogicalDevice, indices.GraphicsFamily, 0, &GraphicsQueue);
    vkGetDeviceQueue(MainDevice.LogicalDevice, indices.PresentationFamily, 0, &PresentationQueue);
}

void xRenderer::CreateSurface()
{
    if(glfwCreateWindowSurface(Instance, Window, nullptr, &Surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create surface");
    }
}

bool xRenderer::CheckInstanceExtensionSupport(std::vector<const char*>* extensions)
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

bool xRenderer::CheckSuitableDevice(VkPhysicalDevice Device)
{
/*
    VkPhysicalDeviceProperties DeviceProperties;
    vkGetPhysicalDeviceProperties(Device, &DeviceProperties);

    VkPhysicalDeviceFeatures DeviceFeatures;
    vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);
*/
    xRenderUtil::QueueFamilyIndices indices = GetQueueFamilies(Device);

    bool bExtensionsSupported = CheckDeviceExtensionSupport(Device);

    bool bSwapChainValid;
    if(bExtensionsSupported)
    {
        xRenderUtil::SwapChainDetails SwapChainDetails = GetSwapChainDetails(Device);
        bSwapChainValid = !SwapChainDetails.Formats.empty() && !SwapChainDetails.PresentationModes.empty();
    }

    return indices.IsValid() && bExtensionsSupported && bSwapChainValid;
}

xRenderUtil::QueueFamilyIndices xRenderer::GetQueueFamilies(VkPhysicalDevice Device)
{
    xRenderUtil::QueueFamilyIndices indices;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, queueFamilies.data());

    i32 idx = 0;
    for(const VkQueueFamilyProperties& queueFamily : queueFamilies)
    {
        if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.GraphicsFamily = idx;
        }

        VkBool32 bPresentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, idx, Surface, &bPresentationSupport);

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

xRenderUtil::SwapChainDetails xRenderer::GetSwapChainDetails(VkPhysicalDevice Device)
{
    xRenderUtil::SwapChainDetails swapChainDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &swapChainDetails.SurfaceCapabilities);

    u32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &formatCount, nullptr);

    if(formatCount != 0)
    {
        swapChainDetails.Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &formatCount, swapChainDetails.Formats.data());
    }

    u32 presentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &presentationModeCount, nullptr);

    if(presentationModeCount != 0)
    {
        swapChainDetails.PresentationModes.resize(presentationModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &presentationModeCount, swapChainDetails.PresentationModes.data());
    }

    return swapChainDetails;
}

bool xRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice Device)
{
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, nullptr);

    if(extensionCount == 0)
    {
        return false;
    }

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, availableExtensions.data());

    for(const char* deviceExtension : xRenderUtil::DeviceExtensions)
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

bool xRenderer::CheckValidationLayerSupport()
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

VkBool32 VKAPI_CALL xRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    std::string message = "Vulkan: [TYPE:";
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    {
        message += "G";
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    {
        message += "V";
    }
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        message += "P";
    }
    message += "] ";

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "Validation Layer: " << message << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

void xRenderer::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoExt;
    PopulateDebugMessengerCreateInfo(debugUtilsMessengerCreateInfoExt);

    if (VkResult result = CreateDebugUtilsMessengerEXT(Instance, &debugUtilsMessengerCreateInfoExt, nullptr, &DebugMessenger); result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult
xRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
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

void xRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void xRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

void xRenderer::Clean()
{
    vkDestroySurfaceKHR(Instance, Surface, nullptr);

    if(EnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
    }

    vkDestroyDevice(MainDevice.LogicalDevice, nullptr);
    vkDestroyInstance(Instance, nullptr);
}

xRenderer::~xRenderer()
{
    Window = nullptr;
}
