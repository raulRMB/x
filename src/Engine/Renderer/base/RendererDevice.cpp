#include <vulkan/vulkan.h>
#include "RendererDevice.h"
#include "../../Core/Window.h"
#include <SDL2/SDL_vulkan.h>
#include <string.h>

namespace x {

    void RendererDevice::Create(VkInstance instance) {
        if(SDL_Vulkan_CreateSurface(Window::Get().GetWindow(), instance, &MainDevice.Surface) != SDL_TRUE)
        {
            throw std::runtime_error("Failed to create surface");
        }
        GetPhysicalDevice(instance);
        CreateLogicalDevice();
    }

    void RendererDevice::Clean(VkInstance instance) {
        vkDestroySurfaceKHR(instance, MainDevice.Surface, nullptr);
        vkDestroyDevice(MainDevice.LogicalDevice, nullptr);
    }

    void RendererDevice::GetPhysicalDevice(VkInstance instance) {
        u32 deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if(deviceCount == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        for(const VkPhysicalDevice& device : devices) {
            if(CheckSuitableDevice(device)) {
                MainDevice.PhysicalDevice = device;
                break;
            }
        }
        if(MainDevice.PhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU");
        }
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(MainDevice.PhysicalDevice, &deviceProperties);
        printf("Using GPU: %s\n", deviceProperties.deviceName);
        MainDevice.MinUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;
    }

    RenderUtil::QueueFamilyIndices RendererDevice::GetQueueFamilies(VkPhysicalDevice device) {
        RenderUtil::QueueFamilyIndices indices;
        u32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        i32 idx = 0;
        for(const VkQueueFamilyProperties& queueFamily : queueFamilies) {
            if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = idx;
            }
            VkBool32 bPresentationSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, MainDevice.Surface, &bPresentationSupport);
            if(queueFamily.queueCount > 0 && bPresentationSupport) {
                indices.PresentationFamily = idx;
            }
            if(indices.IsValid()) {
                break;
            }
            idx++;
        }
        return indices;
    }

    RenderUtil::SurfaceDetails RendererDevice::GetSurfaceDetails(VkPhysicalDevice device)
    {
        RenderUtil::SurfaceDetails SurfaceDetails;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, MainDevice.Surface, &SurfaceDetails.SurfaceCapabilities);
        u32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, MainDevice.Surface, &formatCount, nullptr);
        if(formatCount != 0) {
            SurfaceDetails.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, MainDevice.Surface, &formatCount, SurfaceDetails.Formats.data());
        }
        u32 presentationModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, MainDevice.Surface, &presentationModeCount, nullptr);
        if(presentationModeCount != 0) {
            SurfaceDetails.PresentationModes.resize(presentationModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, MainDevice.Surface, &presentationModeCount, SurfaceDetails.PresentationModes.data());
        }
        return SurfaceDetails;
    }

    bool RendererDevice::CheckSuitableDevice(VkPhysicalDevice device) {
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        x::RenderUtil::QueueFamilyIndices indices = GetQueueFamilies(device);
        bool bExtensionsSupported = CheckDeviceExtensionSupport(device);
        bool bSwapChainValid;
        if(bExtensionsSupported) {
            x::RenderUtil::SurfaceDetails SurfaceDetails = GetSurfaceDetails(device);
            bSwapChainValid = !SurfaceDetails.Formats.empty() && !SurfaceDetails.PresentationModes.empty() && deviceFeatures.samplerAnisotropy;
        }
        return indices.IsValid() && bExtensionsSupported && bSwapChainValid;
    }

    bool RendererDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        u32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        if(extensionCount == 0)
            return false;
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        for(const char* deviceExtension : x::RenderUtil::DeviceExtensions) {
            bool bExtensionFound = false;
            for(const VkExtensionProperties& availableExtension : availableExtensions) {
                if(strcmp(deviceExtension, availableExtension.extensionName) == 0) {
                    bExtensionFound = true;
                    break;
                }
            }
            if(!bExtensionFound)
                return false;
        }
        return true;
    }

    void RendererDevice::CreateLogicalDevice() {
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
        
        #ifdef __APPLE__
        const char* const* currentExtensions = x::RenderUtil::DeviceExtensions.data();
        uint32_t currentExtensionCount = static_cast<uint32_t>(x::RenderUtil::DeviceExtensions.size());
        const char** newExtensions = new const char*[currentExtensionCount + 1];
        for (uint32_t i = 0; i < currentExtensionCount; ++i) {
            newExtensions[i] = currentExtensions[i];
        }
        newExtensions[currentExtensionCount] = "VK_KHR_portability_subset";
        deviceCreateInfo.ppEnabledExtensionNames = newExtensions;
        deviceCreateInfo.enabledExtensionCount = currentExtensionCount + 1;
        #else
        deviceCreateInfo.ppEnabledExtensionNames = x::RenderUtil::DeviceExtensions.data();
        #endif

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        if(vkCreateDevice(MainDevice.PhysicalDevice, &deviceCreateInfo, nullptr, &MainDevice.LogicalDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device");
        }

        vkGetDeviceQueue(MainDevice.LogicalDevice, indices.GraphicsFamily, 0, &MainDevice.GraphicsQueue);
        vkGetDeviceQueue(MainDevice.LogicalDevice, indices.PresentationFamily, 0, &MainDevice.PresentationQueue);
    }

}
