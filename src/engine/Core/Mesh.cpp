#include <stdexcept>
#include "Mesh.h"

xMesh::xMesh(const std::vector<x::RenderUtil::Vertex>& vertices, const std::vector<u32>& indices, i32 textureId, VkQueue transferQueue,
             VkCommandPool transferComandPool, VkPhysicalDevice physicalDevice, VkDevice device)
:
    VertexCount(static_cast<u32>(vertices.size())),
    VertexBufferMemory(VK_NULL_HANDLE),
    IndexCount(static_cast<u32>(indices.size())),
    IndexBufferMemory(VK_NULL_HANDLE),
    PhysicalDevice(physicalDevice),
    Model({glm::mat4(1.0f)}),
    Device(device),
    TextureId(textureId)
{
    VertexBuffer = CreateVertexBuffer(transferQueue, transferComandPool, vertices);
    IndexBuffer = CreateIndexBuffer(transferQueue, transferComandPool, indices);
}

VkBuffer xMesh::CreateVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
                                   const std::vector<x::RenderUtil::Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(x::RenderUtil::Vertex) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    x::RenderUtil::CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), bufferSize);
    vkUnmapMemory(Device, stagingBufferMemory);

    x::RenderUtil::CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);

    x::RenderUtil::CopyBuffer(Device, transferQueue, transferCommandPool, stagingBuffer, VertexBuffer, bufferSize);

    vkDestroyBuffer(Device, stagingBuffer, nullptr);
    vkFreeMemory(Device, stagingBufferMemory, nullptr);

    return VertexBuffer;
}

void xMesh::DestroyVertexBuffer()
{
    vkDestroyBuffer(Device, VertexBuffer, nullptr);
    vkFreeMemory(Device, VertexBufferMemory, nullptr);
}

VkBuffer xMesh::CreateIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
                                  const std::vector<u32> &indices)
{
    VkDeviceSize bufferSize = sizeof(u32) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    x::RenderUtil::CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), bufferSize);
    vkUnmapMemory(Device, stagingBufferMemory);

    x::RenderUtil::CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory);

    x::RenderUtil::CopyBuffer(Device, transferQueue, transferCommandPool, stagingBuffer, IndexBuffer, bufferSize);

    vkDestroyBuffer(Device, stagingBuffer, nullptr);
    vkFreeMemory(Device, stagingBufferMemory, nullptr);

    return IndexBuffer;
}

void xMesh::DestroyIndexBuffer()
{
    vkDestroyBuffer(Device, IndexBuffer, nullptr);
    vkFreeMemory(Device, IndexBufferMemory, nullptr);
}

void xMesh::DestroyBuffers()
{
    DestroyVertexBuffer();
    DestroyIndexBuffer();
}

xMesh::~xMesh() = default;
