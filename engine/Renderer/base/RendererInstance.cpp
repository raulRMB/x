#include "../../Core/defines.h"
#include "../../Core/Window.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>
#include "RendererInstance.h"

    void RendererInstance::Clean()
    {
        if(EnableValidationLayers)
        {
            RendererInstance::DestroyDebugUtilsMessengerEXT(Instance, RendererInstance::DebugMessenger, nullptr);
        }
        vkDestroyInstance(Instance, nullptr);
    }

    void RendererInstance::Create()
    {
        if(EnableValidationLayers && !CheckValidationLayerSupport(ValidationLayers))
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
        SDL_Vulkan_GetInstanceExtensions(x::Window::Get().GetWindow(), &extensionCount, nullptr);
        instanceExtensions.resize(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(x::Window::Get().GetWindow(), &extensionCount, instanceExtensions.data());
        if(!RendererInstance::CheckInstanceExtensionSupport(&instanceExtensions))
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
        RendererInstance::SetupDebugMessenger();
    }

    bool RendererInstance::CheckInstanceExtensionSupport(std::vector<const char*>* extensions)
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

    bool RendererInstance::CheckValidationLayerSupport(std::vector<const char *> ValidationLayers) {
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

    void RendererInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }

    void RendererInstance::SetupDebugMessenger() {
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

    VkResult RendererInstance::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const VkAllocationCallbacks *pAllocator,
                                        VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto pFunction = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        return pFunction == nullptr ? VK_ERROR_EXTENSION_NOT_PRESENT : pFunction(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    VkBool32 VKAPI_CALL RendererInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

    void RendererInstance::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }