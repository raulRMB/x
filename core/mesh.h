//
// Created by Raul Romero on 2023-09-30.
//

#ifndef X_MESH_H
#define X_MESH_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <base/defines.h>
#include <vector>
#include "engine/rutil.h"

class xMesh
{
private:
    u32 VertexCount;
    VkBuffer VertexBuffer;
    VkDeviceMemory VertexBufferMemory;

    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;

    VkBuffer CreateVertexBuffer(const std::vector<xRUtil::Vertex>& vertices);

    u32 FindMemoryTypeIndex(u32 bits, VkMemoryPropertyFlags propertyFlagBits);
public:
    xMesh() = default;
    xMesh(const std::vector<xRUtil::Vertex>& vertices, VkPhysicalDevice physicalDevice, VkDevice device);
    ~xMesh();

    [[nodiscard]] inline u32 GetVertexCount() const { return VertexCount; }
    [[nodiscard]] inline VkBuffer GetVertexBuffer() const { return VertexBuffer; }
    void DestroyVertexBuffer();
};


#endif //X_MESH_H
