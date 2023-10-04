//
// Created by Raul Romero on 2023-09-30.
//

#include <stdexcept>
#include "mesh.h"

xMesh::xMesh(const std::vector<xRUtil::Vertex>& vertices, VkPhysicalDevice physicalDevice, VkDevice device)
:
    VertexCount(static_cast<u32>(vertices.size())),
    VertexBufferMemory(VK_NULL_HANDLE),
    PhysicalDevice(physicalDevice),
    Device(device)
{
    VertexBuffer = CreateVertexBuffer(vertices);
}

VkBuffer xMesh::CreateVertexBuffer(const std::vector<xRUtil::Vertex>& vertices)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = sizeof(xRUtil::Vertex) * VertexCount;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(Device, &bufferCreateInfo, nullptr, &VertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vertex buffer!");
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(Device, VertexBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(Device, &memoryAllocateInfo, nullptr, &VertexBufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(Device, VertexBuffer, VertexBufferMemory, 0);

    void* data;
    vkMapMemory(Device, VertexBufferMemory, 0, bufferCreateInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferCreateInfo.size);
    vkUnmapMemory(Device, VertexBufferMemory);

    return VertexBuffer;
}

void xMesh::DestroyVertexBuffer()
{
    vkDestroyBuffer(Device, VertexBuffer, nullptr);
    vkFreeMemory(Device, VertexBufferMemory, nullptr);
}

u32 xMesh::FindMemoryTypeIndex(u32 bits, VkMemoryPropertyFlags propertyFlagBits)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &memProperties);

    for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((bits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlagBits) == propertyFlagBits)
        {
            return i;
        }
    }

    return -1;
}

xMesh::~xMesh() = default;
