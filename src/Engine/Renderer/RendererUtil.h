#ifndef X_RUTIL_H
#define X_RUTIL_H

#include "../../base/defines.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <../../../vendor/stb_image/stb_image.h>
#include "../../Core/Camera.h"
#include "../../Core/Game.h"

namespace x::RenderUtil
{
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const int MAX_OBJECTS = 100;

    struct Vertex
    {
        v3 Pos;
        v4 Col;
        v2 Tex;
    };

    struct SkeletalVertex
    {
        v3 Pos;
        v4 Col;
        v2 Tex;
        iv4 BoneIds;
        v4 BoneWeights = v4(0.0f);
    };

    const std::vector<const char*> DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct QueueFamilyIndices
    {
        i32 GraphicsFamily = -1;
        i32 PresentationFamily = -1;

        [[nodiscard]] bool IsValid() const
        {
            return GraphicsFamily >= 0 && PresentationFamily >= 0;
        }
    };

    struct SurfaceDetails
    {
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentationModes;
    };

    struct SwapChainImage
    {
        VkImage Image;
        VkImageView ImageView;
    };

    inline static u32 FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, u32 bits, VkMemoryPropertyFlags propertyFlagBits)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((bits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlagBits) == propertyFlagBits)
            {
                return i;
            }
        }

        return -1;
    }

    inline void CreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    inline static VkCommandBuffer BeginCommandBuffer(VkDevice device, VkCommandPool commandPool)
    {
        VkCommandBuffer commandBuffer;

        VkCommandBufferAllocateInfo allocInfoP{};
        allocInfoP.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfoP.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfoP.commandPool = commandPool;
        allocInfoP.commandBufferCount = 1;

        vkAllocateCommandBuffers(device, &allocInfoP, &commandBuffer);

        VkCommandBufferBeginInfo beginInfoP{};
        beginInfoP.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoP.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfoP);

        return commandBuffer;
    }

    inline static void EndAndSubmtCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfoP{};
        submitInfoP.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfoP.commandBufferCount = 1;
        submitInfoP.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(queue, 1, &submitInfoP, VK_NULL_HANDLE);
        vkQueueWaitIdle(queue);

        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    inline static void CopyBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool commandPool, VkBuffer srcBuffer,
                           VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer transferCommandBuffer = BeginCommandBuffer(device, commandPool);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndAndSubmtCommandBuffer(device, commandPool, transferQueue, transferCommandBuffer);
    }

    inline static void CopyImageBuffer(VkDevice device, VkQueue transferQueue, VkCommandPool transferCommandPool,
                                VkBuffer srcBuffer, VkImage dstImage, u32 width, u32 height)
    {
        VkCommandBuffer transferCommandBuffer = BeginCommandBuffer(device, transferCommandPool);

        VkBufferImageCopy imageRegion{};
        imageRegion.bufferOffset = 0;
        imageRegion.bufferRowLength = 0;
        imageRegion.bufferImageHeight = 0;
        imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageRegion.imageSubresource.mipLevel = 0;
        imageRegion.imageSubresource.baseArrayLayer = 0;
        imageRegion.imageSubresource.layerCount = 1;
        imageRegion.imageOffset = {0, 0, 0};
        imageRegion.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(transferCommandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                               &imageRegion);

        EndAndSubmtCommandBuffer(device, transferCommandPool, transferQueue, transferCommandBuffer);
    }

    inline static void TransitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkCommandBuffer commandBuffer = BeginCommandBuffer(device, commandPool);

        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.levelCount = 1;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags srcStage;
        VkPipelineStageFlags dstStage;

        if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = 0;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        vkCmdPipelineBarrier(commandBuffer,
                             srcStage, dstStage,
                             0,
                             0, nullptr,
                             0, nullptr,
                             1, &imageMemoryBarrier);

        EndAndSubmtCommandBuffer(device, commandPool, queue, commandBuffer);
    }

    inline VkImage CreateImage(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags useFlags,
                        VkMemoryPropertyFlags propFlags, VkDeviceMemory& imageMemory, VkDevice device,
                        VkPhysicalDevice physicalDevice)
    {
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = useFlags;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkImage image;
        if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryTypeIndex(physicalDevice, memRequirements.memoryTypeBits, propFlags);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, image, imageMemory, 0);

        return image;
    }

    inline VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectFlags, VkDevice device)
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
        if(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }

        return imageView;
    }

    inline stbi_uc* LoadTextureFile(const std::string& fileName, i32* width, i32* height, VkDeviceSize* imageSize)
    {
        i32 channels;

        std::string fileLoc = "../assets/textures/" + fileName;
        stbi_uc* image;
        if(image = stbi_load(fileLoc.c_str(), width, height, &channels, STBI_rgb_alpha); !image)
        {
            throw std::runtime_error("Failed to load a texture file! (" + fileName + ")");
        }

        *imageSize = *width * *height * 4;

        return image;
    }

    inline v3 GetMouseWorldPosition(bool atZeroZ = false)
    {
        i32 x, y;
        SDL_GetMouseState(&x, &y);

        double x_ndc = (2.f * (f32)x / (f32)WINDOW_WIDTH) - 1.f;
        double y_ndc = (2.f * (f32)y / (f32)WINDOW_HEIGHT) - 1.f;
        m4 viewProjectionInverse = CameraSystem::Get().GetVPI();
        v4 worldSpacePosition(x_ndc, y_ndc, atZeroZ ? 0.f : 1.f, 1.f);
        auto world = viewProjectionInverse * worldSpacePosition;
        world /= world.w;
        return world;
    }
};


#endif //X_RUTIL_H
