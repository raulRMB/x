//
// Created by Raul Romero on 2023-09-30.
//

#ifndef X_MESH_COMPONENT_H
#define X_MESH_COMPONENT_H

#include <vulkan/vulkan.h>
#include <base/defines.h>
#include <vector>
#include "engine/rutil.h"

struct xModel
{
    glm::mat4 Model;

    xModel() = default;
    explicit xModel(const glm::mat4& model) : Model(model) {}
    operator const glm::mat4()& { return Model; }
    operator glm::mat4()& { return Model; }
};

class xMesh
{
private:
    xModel Model;
    i32 TextureId = -1;

    u32 VertexCount;
    VkBuffer VertexBuffer;
    VkDeviceMemory VertexBufferMemory;

    u32 IndexCount;
    VkBuffer IndexBuffer;
    VkDeviceMemory IndexBufferMemory;

    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;

protected:
    VkBuffer CreateVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
                                const std::vector<x::RenderUtil::Vertex>& vertices);
    VkBuffer CreateIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
                               const std::vector<u32>& indices);
    void DestroyVertexBuffer();
    void DestroyIndexBuffer();
public:
    xMesh() = default;

    xMesh(const std::vector<x::RenderUtil::Vertex>& vertices, const std::vector<u32>& indices, i32 textureId, VkQueue transferQueue,
          VkCommandPool transferComandPool, VkPhysicalDevice physicalDevice, VkDevice device);
    ~xMesh();

    [[nodiscard]] i32 inline GetTextureId() const { return TextureId; }

    inline void SetModel(const glm::mat4& model) { Model.Model = model; }
    [[nodiscard]] inline const xModel& GetModel() const { return Model; }

    [[nodiscard]] inline u32 GetVertexCount() const { return VertexCount; }
    [[nodiscard]] inline VkBuffer GetVertexBuffer() const { return VertexBuffer; }

    [[nodiscard]] inline u32 GetIndexCount() const { return IndexCount; }
    [[nodiscard]] inline VkBuffer GetIndexBuffer() const { return IndexBuffer; }

    void DestroyBuffers();
};


#endif //X_MESH_COMPONENT_H
