//
// Created by Raul Romero on 2023-09-22.
//

#ifndef X_RENDERER_H
#define X_RENDERER_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "base/defines.h"

#include <stdexcept>
#include <vector>
#include <set>

#include "rutil.h"
#include "core/mesh.h"

#include "util/primitives.h"

namespace x
{
    struct Camera
    {
        glm::vec3 Position;
        glm::vec3 Forward;
        glm::vec3 Up;
        float FOV;
        glm::mat4 View;

        Camera() = default;

        Camera(const glm::vec3 &position, const glm::vec3 &forward, const glm::vec3 &up, float fov)
                : Position(position), Forward(forward), Up(up), FOV(fov), View(glm::mat4(1.f))
        {}
    };

    class Window;

    class Renderer
    {
    private:
        friend class Engine;

        Window *Window;

        VkInstance Instance;

        int CurrentFrame = 0;

        std::vector<class xMesh> MeshList;

        Camera Camera;

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

        VkFormat DepthBufferImageFormat;
        VkImage DepthBufferImage;
        VkDeviceMemory DepthBufferImageMemory;
        VkImageView DepthBufferImageView;

        std::vector<VkImage> TextureImages;
        std::vector<VkDeviceMemory> TextureImageMemory;
        std::vector<VkImageView> TextureImageViews;
        VkSampler TextureSampler;

        VkQueue GraphicsQueue;
        VkQueue PresentationQueue;

        VkDescriptorSetLayout DescriptorSetLayout;
        VkDescriptorSetLayout SamplerSetLayout;
        VkPushConstantRange PushConstantRange;

        VkDescriptorPool DescriptorPool;
        VkDescriptorPool SamplerDescriptorPool;
        std::vector<VkDescriptorSet> DescriptorSets;
        std::vector<VkDescriptorSet> SamplerDescriptorSets;

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

        const std::vector<const char *> ValidationLayers = {
                "VK_LAYER_KHRONOS_validation"
        };

#ifdef NDEBUG
        const bool EnableValidationLayers = false;
#else
        const bool EnableValidationLayers = true;
#endif

        VkDebugUtilsMessengerEXT DebugMessenger;

    private:
        Renderer();

        ~Renderer();

        i32 Init(x::Window *window);

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

        void CreateDepthBufferImage();

        void CreateFramebuffers();

        void CreateGraphicsCommandPool();

        void CreateCommandBuffers();

        void CreateSynchronization();

        void CreateTextureSampler();

        void CreateUniformBuffers();

        void CreateDescriptorPool();

        void CreateDescriptorSets();

        void CreateMeshes();

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
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                [[maybe_unused]] void *pUserData);

        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        static VkResult CreateDebugUtilsMessengerEXT(
                VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                const VkAllocationCallbacks *pAllocator,
                VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void DestroyDebugUtilsMessengerEXT(
                VkInstance instance,
                VkDebugUtilsMessengerEXT debugMessenger,
                const VkAllocationCallbacks *pAllocator);

        xRUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);

        xRUtil::SwapChainDetails GetSwapChainDetails(VkPhysicalDevice device);

        static VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);

        static VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);

        VkFormat ChooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling,
                                       VkFormatFeatureFlags featureFlags);

        [[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<byte> &bytes) const;

        void CreateSurfaceGLFW();

        void CreateSurfaceSDL();

        i32 CreateTextureImage(const std::string &fileName);

        i32 CreateTexture(const std::string &fileName);

        i32 CreateTextureDescriptor(VkImageView textureImage);

    public:
        void CreateMesh(const std::string& texture, X::Primitives2D::Shape shape = X::Primitives2D::Shape::Circle,
                        const v4& color = {1.f, 1.f, 1.f, 1.f});

        void UpdateCamera(const v3& pos);

        const struct Camera& GetCamera() { return Camera; }
        const UBOViewProjection& GetUBOViewProjection() { return UboViewProjection; }

        glm::mat4 GetViewProjectionInverse();
    };
}

#endif //X_RENDERER_H
