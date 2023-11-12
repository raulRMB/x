#ifndef X_RENDERER_H
#define X_RENDERER_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "../Core/defines.h"

#include <stdexcept>
#include <vector>
#include <set>

#include "RendererUtil.h"
#include "../Core/Mesh.h"
#include "../Core/MeshModel.h"

#include "../Util/Primitives.h"

#include <imgui.h>
#include "../Core/SkeletalMesh.h"
#include "Base/RendererInstance.h"
#include "Base/RendererDevice.h"
#include "Base/SwapChain.h"

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

    int CurrentFrame = 0;
    RendererInstance* RenderInstance = new RendererInstance();
    RendererDevice* RenderDevice = new RendererDevice();
    SwapChain* swapchain = new SwapChain();
    RenderUtil::Device MainDevice;
    std::vector<xMesh> MeshList;
    std::vector<MeshModel> ModelList;
    std::vector<SkeletalMesh> SkeletalMeshList;

    struct UBOViewProjection
    {
        glm::mat4 Projection;
        glm::mat4 View;
    } UboViewProjection;

    std::vector<VkCommandBuffer> CommandBuffers;

    VkFormat DepthBufferImageFormat;
    VkImage DepthBufferImage;
    VkDeviceMemory DepthBufferImageMemory;
    VkImageView DepthBufferImageView;

    std::vector<VkImage> TextureImages;
    std::vector<VkDeviceMemory> TextureImageMemory;
    std::vector<VkImageView> TextureImageViews;
    VkSampler TextureSampler;

    VkDescriptorSetLayout DescriptorSetLayout;
    VkDescriptorSetLayout SamplerSetLayout;
    VkDescriptorSetLayout BoneDescriptorSetLayout;
    VkPushConstantRange PushConstantRange;

    VkDescriptorPool ImguiPool;

    VkDescriptorPool DescriptorPool;
    VkDescriptorPool SamplerDescriptorPool;
    std::vector<VkDescriptorSet> DescriptorSets;
    std::vector<VkDescriptorSet> BoneDescriptorSets;
    std::vector<VkDescriptorSet> SamplerDescriptorSets;

    std::vector<VkBuffer> VPUniformBuffers;
    std::vector<VkDeviceMemory> VPUniformBuffersMemory;

    size_t BoneUniformAlignment;
    BoneTransforms* BoneTransferSpace;

    std::vector<VkBuffer> BoneDUniformBuffers;
    std::vector<VkDeviceMemory> BoneDUniformBuffersMemory;

    VkDescriptorPool BoneDescriptorPool;

    VkPipelineLayout PipelineLayout;
    VkRenderPass RenderPass;
    VkPipeline GraphicsPipeline;
    VkPipeline LinePipeline;

    VkPipelineLayout SkeletalPipelineLayout;
    VkPipeline SkeletalPipeline;

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

private:
    Renderer();
    ~Renderer();

    i32 Init();

    void UpdateModel(u32 modelId, glm::mat4 newModel);

    void DrawFrame();

    void Clean();

private:
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

    void AllocateDynamicBufferTransferSpace();

    static VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);

    static VkPresentModeKHR ChooseBestPresentationMode(const std::vector<VkPresentModeKHR> &presentationModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);

    VkFormat ChooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling,
                                   VkFormatFeatureFlags featureFlags);

    [[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char>& bytes) const;

    i32 CreateTextureImage(const std::string &fileName);

    i32 CreateTexture(const std::string &fileName);

    i32 CreateTextureDescriptor(VkImageView textureImage);
public:

    void CreateMesh(const std::string& texture, x::Primitives2D::Shape shape = x::Primitives2D::Shape::Circle,
                    const v4& color = {1.f, 1.f, 1.f, 1.f});
    u32 CreateMeshModel(const std::string& fileName);

    void CreateSkeletalMesh(const std::string& fileName);

    u32 CreateLine(const v3& start, const v3& end, const v4& color = {1.f, 1.f, 1.f, 1.f});

    [[nodiscard]] const UBOViewProjection& GetUBOViewProjection() const { return UboViewProjection; }

    glm::mat4 GetViewProjectionInverse();
private:
    void InitImGui();

    void CreateSkeletalPipelineLayout();
};
}

#endif //X_RENDERER_H
