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
#include "core/mesh.h"

class xWindow;

class xRenderer
{
private:
    friend class xEngine;

    xWindow* Window;

    VkInstance Instance;

    int CurrentFrame = 0;

    std::vector<xMesh> MeshList;

    struct
    {
        VkPhysicalDevice PhysicalDevice;
        VkDevice LogicalDevice;
    } MainDevice;

    VkSurfaceKHR Surface;

    struct UBOViewProjection
    {
        glm::mat4 Projection;
        glm::mat4 View;
    } UboViewProjection;

    VkSwapchainKHR Swapchain;
    VkFormat SwapchainImageFormat;
    VkExtent2D SwapchainExtent;
    std::vector<xRUtil::SwapChainImage> SwapchainImages;
    std::vector<VkFramebuffer> SwapchainFramebuffers;
    std::vector<VkCommandBuffer> CommandBuffers;

    VkQueue GraphicsQueue;
    VkQueue PresentationQueue;

    VkDescriptorSetLayout DescriptorSetLayout;
    VkPushConstantRange PushConstantRange;

    VkDescriptorPool DescriptorPool;
    std::vector<VkDescriptorSet> DescriptorSets;

//    VkDeviceSize MinUniformBufferOffset;
//    size_t ModelUniformAlignment;
//    class xModel* ModelTransferSpace;

    std::vector<VkBuffer> VPUniformBuffers;
    std::vector<VkDeviceMemory> VPUniformBuffersMemory;

//    std::vector<VkBuffer> ModelDUniformBuffers;
//    std::vector<VkDeviceMemory> ModelDUniformBuffersMemory;


    VkPipelineLayout PipelineLayout;
    VkRenderPass RenderPass;
    VkPipeline GraphicsPipeline;

    VkCommandPool GraphicsCommandPool;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;

    std::vector<VkFence> DrawFences;

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

    i32 Init(xWindow* window);

    void UpdateModel(u32 modelId, glm::mat4 newModel);

    void DrawFrame();

    void Clean();

private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void GetPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapChain();
    void CreateDescriptorSetLayout();
    void CreatePushConstantRange();
    void CreateGraphicsPipeline();
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateGraphicsCommandPool();
    void CreateCommandBuffers();
    void CreateSynchronization();

    void CreateUniformBuffers();

    void CreateDescriptorPool();
    void CreateDescriptorSets();

    void UpdateUniformBuffers(u32 imageIndex);

    void RecordCommands(u32 currentImage);

//    void AllocateDynamicBufferTransferSpace();

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

    [[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<byte>& bytes) const;

    void CreateSurfaceGLFW();
    void CreateSurfaceSDL();
};


#endif //X_RENDERER_H
