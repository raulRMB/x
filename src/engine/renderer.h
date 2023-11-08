#ifndef X_RENDERER_H
#define X_RENDERER_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "../base/defines.h"

#include <stdexcept>
#include <vector>
#include <set>

#include "rutil.h"
#include "../core/mesh.h"
#include "../core/MeshModel.h"

#include "../util/primitives.h"

#include <imgui.h>
#include "../core/SkeletalMesh.h"

namespace x
{
    class Window;

    class Renderer
    {
    public:
        static Renderer &Get();

        u32 CreateTriangle(v2 vec1, v2 vec2, v2 vec3, glm::vec4 vec4);

        SkeletalMesh& GetSkeletalMesh(u32 id);

    private:
        friend class Engine;

        VkInstance Instance;

        int CurrentFrame = 0;

        std::vector<xMesh> MeshList;
        std::vector<MeshModel> ModelList;
        std::vector<SkeletalMesh> SkeletalMeshList;

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
        std::vector<x::RenderUtil::SwapChainImage> SwapchainImages;
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

        VkDescriptorPool ImguiPool;

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
        VkPipeline LinePipeline;

        VkPipelineLayout SkeletalPipelineLayout;
        VkPipeline SkeletalPipeline;

//        VkPushConstantRange SkeletalPushConstantRange;

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

        i32 Init();

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

        void CreatePipelineLayout();

        void CreatePipeline(VkPipeline &pipeline, VkBool32 depthTestEnable = VK_TRUE, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        void CreateSkeletalPipeline();

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

        x::RenderUtil::QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice device);

        x::RenderUtil::SwapChainDetails GetSwapChainDetails(VkPhysicalDevice device);

        static VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);

        static VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);

        VkFormat ChooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling,
                                       VkFormatFeatureFlags featureFlags);

        [[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char>& bytes) const;

        void CreateSurfaceGLFW();

        void CreateSurfaceSDL();

        i32 CreateTextureImage(const std::string &fileName);

        i32 CreateTexture(const std::string &fileName);

        i32 CreateTextureDescriptor(VkImageView textureImage);
    public:

        void CreateMesh(const std::string& texture, x::Primitives2D::Shape shape = x::Primitives2D::Shape::Circle,
                        const v4& color = {1.f, 1.f, 1.f, 1.f});
        u32 CreateMeshModel(const std::string& fileName);

        void CreateSkeletalMesh(const std::string& fileName);

        u32 CreateLine(const v3& start, const v3& end, const v4& color = {1.f, 1.f, 1.f, 1.f});

        const UBOViewProjection& GetUBOViewProjection() { return UboViewProjection; }

        glm::mat4 GetViewProjectionInverse();
    private:
        void InitImGui();

        void CreateSkeletalPipelineLayout();
    };
}

#endif //X_RENDERER_H
