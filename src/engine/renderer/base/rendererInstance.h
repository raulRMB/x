#pragma once

#include "../../../base/defines.h"
#include "../../window.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>

namespace x {
class rendererInstance {

public:
    void create();
    void clean();
    VkInstance get() { return Instance; }
    rendererInstance() {}

private:
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;
    const std::vector<const char *> ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool EnableValidationLayers = false;
#else
    const bool EnableValidationLayers = true;
#endif

    bool CheckInstanceExtensionSupport(std::vector<const char*>* extensions);
    bool CheckValidationLayerSupport(std::vector<const char *> ValidationLayers);
    void SetupDebugMessenger();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                        const VkAllocationCallbacks *pAllocator,
                                        VkDebugUtilsMessengerEXT *pDebugMessenger);
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                             [[maybe_unused]] void *pUserData);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks *pAllocator);
};
}