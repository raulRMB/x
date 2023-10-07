//
// Created by Raul Romero on 2023-09-23.
//

#ifndef X_RUTIL_H
#define X_RUTIL_H

#include <base/defines.h>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace xRUtil
{
    const int MAX_FRAMES_IN_FLIGHT = 2;
    const int MAX_OBJECTS = 20;

    struct Vertex
    {
        glm::vec3 Pos;
        glm::vec4 Col;
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

    struct SwapChainDetails
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

    static u32 FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, u32 bits, VkMemoryPropertyFlags propertyFlagBits)
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

    static void CopyBuffer(VkDevice device, VkQueue TransferQueue, VkCommandPool commandPool, VkBuffer srcBuffer,
                           VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer transferCommandBuffer;

        VkCommandBufferAllocateInfo allocInfoP{};
        allocInfoP.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfoP.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfoP.commandPool = commandPool;
        allocInfoP.commandBufferCount = 1;

        vkAllocateCommandBuffers(device, &allocInfoP, &transferCommandBuffer);

        VkCommandBufferBeginInfo beginInfoP{};
        beginInfoP.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfoP.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(transferCommandBuffer, &beginInfoP);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(transferCommandBuffer);

        VkSubmitInfo submitInfoP{};
        submitInfoP.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfoP.commandBufferCount = 1;
        submitInfoP.pCommandBuffers = &transferCommandBuffer;

        vkQueueSubmit(TransferQueue, 1, &submitInfoP, VK_NULL_HANDLE);
        vkQueueWaitIdle(TransferQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &transferCommandBuffer);
    }
};


#endif //X_RUTIL_H
