//
// Created by Raul Romero on 2023-09-22.
//

#include <cstring>
#include <iostream>
#include <limits>
#include <array>
#include "renderer.h"
#include "util/file.h"

xRenderer::xRenderer() :
        Window(nullptr),
        Instance(VK_NULL_HANDLE),
        MainDevice({VK_NULL_HANDLE, VK_NULL_HANDLE}),
        Surface(VK_NULL_HANDLE),
        Swapchain(VK_NULL_HANDLE),
        SwapchainImageFormat(VK_FORMAT_UNDEFINED),
        SwapchainExtent({0, 0}),
        SwapchainFramebuffers(std::vector<VkFramebuffer>()),
        CommandBuffers(std::vector<VkCommandBuffer>()),
        RenderFinishedSemaphore(VK_NULL_HANDLE),
        ImageAvailableSemaphore(VK_NULL_HANDLE),
        GraphicsCommandPool(VK_NULL_HANDLE),
        GraphicsQueue(VK_NULL_HANDLE),
        PresentationQueue(VK_NULL_HANDLE),
        DebugMessenger(VK_NULL_HANDLE),
        RenderPass(VK_NULL_HANDLE),
        PipelineLayout(VK_NULL_HANDLE),
        GraphicsPipeline(VK_NULL_HANDLE)
{}

i32 xRenderer::Init(GLFWwindow *window)
{
    Window = window;

    try
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        GetPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateGraphicsCommandPool();
        CreateCommandBuffers();
        RecordCommands();
        CreateSynchronization();
    }
    catch (const std::runtime_error& e)
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
    xRUtil::QueueFamilyIndices indices = GetQueueFamilies(MainDevice.PhysicalDevice);

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
    deviceCreateInfo.enabledExtensionCount = (u32)xRUtil::DeviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = xRUtil::DeviceExtensions.data();

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if(vkCreateDevice(MainDevice.PhysicalDevice, &deviceCreateInfo, nullptr, &MainDevice.LogicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }

    vkGetDeviceQueue(MainDevice.LogicalDevice, indices.GraphicsFamily, 0, &GraphicsQueue);
    vkGetDeviceQueue(MainDevice.LogicalDevice, indices.PresentationFamily, 0, &PresentationQueue);
}

void xRenderer::CreateSwapChain()
{
    xRUtil::SwapChainDetails swapChainDetails = GetSwapChainDetails(MainDevice.PhysicalDevice);

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

    xRUtil::QueueFamilyIndices indices = GetQueueFamilies(MainDevice.PhysicalDevice);

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
        xRUtil::SwapChainImage swapChainImage = {};
        swapChainImage.Image = image;
        swapChainImage.ImageView = CreateImageView(image, SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

        SwapchainImages.push_back(swapChainImage);
    }
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

bool xRenderer::CheckSuitableDevice(VkPhysicalDevice device)
{
/*
    VkPhysicalDeviceProperties DeviceProperties;
    vkGetPhysicalDeviceProperties(Device, &DeviceProperties);

    VkPhysicalDeviceFeatures DeviceFeatures;
    vkGetPhysicalDeviceFeatures(Device, &DeviceFeatures);
*/
    xRUtil::QueueFamilyIndices indices = GetQueueFamilies(device);

    bool bExtensionsSupported = CheckDeviceExtensionSupport(device);

    bool bSwapChainValid;
    if(bExtensionsSupported)
    {
        xRUtil::SwapChainDetails SwapChainDetails = GetSwapChainDetails(device);
        bSwapChainValid = !SwapChainDetails.Formats.empty() && !SwapChainDetails.PresentationModes.empty();
    }

    return indices.IsValid() && bExtensionsSupported && bSwapChainValid;
}

xRUtil::QueueFamilyIndices xRenderer::GetQueueFamilies(VkPhysicalDevice device)
{
    xRUtil::QueueFamilyIndices indices;

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

xRUtil::SwapChainDetails xRenderer::GetSwapChainDetails(VkPhysicalDevice device)
{
    xRUtil::SwapChainDetails swapChainDetails;

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

bool xRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    if(extensionCount == 0)
    {
        return false;
    }

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    for(const char* deviceExtension : xRUtil::DeviceExtensions)
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

void xRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

void xRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkSurfaceFormatKHR xRenderer::ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
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

VkPresentModeKHR xRenderer::ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes)
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

VkExtent2D xRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities)
{
    if(surfaceCapabilities.currentExtent.width != std::numeric_limits<u32>::max())
    {
        return surfaceCapabilities.currentExtent;
    }

    i32 width, height;
    glfwGetFramebufferSize(Window, &width, &height);

    VkExtent2D newExtent = {};
    newExtent.width = static_cast<u32>(width);
    newExtent.height = static_cast<u32>(height);

    newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
    newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

    return newExtent;
}

VkImageView xRenderer::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectFlags) const
{
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if(vkCreateImageView(MainDevice.LogicalDevice, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image view");
    }

    return imageView;
}

void xRenderer::Clean()
{
    vkDeviceWaitIdle(MainDevice.LogicalDevice);

    vkDestroySemaphore(MainDevice.LogicalDevice, RenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(MainDevice.LogicalDevice, ImageAvailableSemaphore, nullptr);

    vkDestroyCommandPool(MainDevice.LogicalDevice, GraphicsCommandPool, nullptr);

    for(const VkFramebuffer& framebuffer : SwapchainFramebuffers)
    {
        vkDestroyFramebuffer(MainDevice.LogicalDevice, framebuffer, nullptr);
    }

    vkDestroyPipeline(MainDevice.LogicalDevice, GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(MainDevice.LogicalDevice, PipelineLayout, nullptr);
    vkDestroyRenderPass(MainDevice.LogicalDevice, RenderPass, nullptr);

    for(const xRUtil::SwapChainImage& swapChainImage : SwapchainImages)
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

xRenderer::~xRenderer()
{
    Window = nullptr;
}

void xRenderer::CreateGraphicsPipeline()
{
    std::vector<char> vertShaderCode = xUtil::xFile::ReadAsBin("../shaders/vert.spv");
    std::vector<char> fragShaderCode = xUtil::xFile::ReadAsBin("../shaders/frag.spv");

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

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
    rasterizationCreateInfo.lineWidth = 1.0f;
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

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;

    if(vkCreatePipelineLayout(MainDevice.LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = (u32)std::size(shaderStages);
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = PipelineLayout;
    pipelineCreateInfo.renderPass = RenderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(MainDevice.LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    vkDestroyShaderModule(MainDevice.LogicalDevice, VertexShaderModule, nullptr);
    vkDestroyShaderModule(MainDevice.LogicalDevice, FragmentShaderModule, nullptr);
}

VkShaderModule xRenderer::CreateShaderModule(const std::vector<byte>& bytes) const
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

void xRenderer::CreateRenderPass()
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

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;

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

    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = (u32)subpassDependencies.size();
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    if(vkCreateRenderPass(MainDevice.LogicalDevice, &renderPassCreateInfo, nullptr, &RenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render pass");
    }
}

void xRenderer::CreateFramebuffers()
{
    SwapchainFramebuffers.resize(SwapchainImages.size());

    for(size_t i = 0; i < SwapchainFramebuffers.size(); i++)
    {
        std::array<VkImageView, 1> attachments = {SwapchainImages[i].ImageView};

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

void xRenderer::CreateGraphicsCommandPool()
{
    xRUtil::QueueFamilyIndices queueFamilyIndices = GetQueueFamilies(MainDevice.PhysicalDevice);

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily;

    if(vkCreateCommandPool(MainDevice.LogicalDevice, &commandPoolCreateInfo, nullptr, &GraphicsCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics command pool");
    }
}

void xRenderer::CreateCommandBuffers()
{
    CommandBuffers.resize(SwapchainFramebuffers.size());

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = GraphicsCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = (u32)CommandBuffers.size();

    if(vkAllocateCommandBuffers(MainDevice.LogicalDevice, &commandBufferAllocateInfo, CommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void xRenderer::RecordCommands()
{
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = RenderPass;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = SwapchainExtent;

    VkClearValue clearValue = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    for(size_t i = 0; i < CommandBuffers.size(); i++)
    {
        renderPassBeginInfo.framebuffer = SwapchainFramebuffers[i];

        if(vkBeginCommandBuffer(CommandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer");
        }

            vkCmdBeginRenderPass(CommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);

                vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(CommandBuffers[i]);

        if(vkEndCommandBuffer(CommandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer");
        }
    }
}

void xRenderer::DrawFrame()
{
    u32 imageIndex;
    vkAcquireNextImageKHR(MainDevice.LogicalDevice, Swapchain, std::numeric_limits<u64>::max(), ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {ImageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = (u32)std::size(waitSemaphores);
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CommandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {RenderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = (u32)std::size(signalSemaphores);
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
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
}

void xRenderer::CreateSynchronization()
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if(vkCreateSemaphore(MainDevice.LogicalDevice, &semaphoreCreateInfo, nullptr, &ImageAvailableSemaphore) != VK_SUCCESS
       || vkCreateSemaphore(MainDevice.LogicalDevice, &semaphoreCreateInfo, nullptr, &RenderFinishedSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create semaphores");
    }
}
