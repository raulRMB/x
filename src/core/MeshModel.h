//
// Created by Raul Romero on 2023-10-13.
//

#ifndef X_MESH_MODEL_H
#define X_MESH_MODEL_H

#include <vector>
#include "../base/defines.h"
#include "mesh.h"

class MeshModel
{
public:
    MeshModel();
    MeshModel(std::vector<xMesh> meshList);

    static std::vector<std::string> LoadMaterials(const class aiScene* scene);
    static std::vector<xMesh> LoadNode(VkPhysicalDevice physicalDevice, VkDevice device,
                                       VkQueue transferQueue, VkCommandPool transferCommandPool,
                                       const class aiNode* node, const class aiScene* scene,
                                       const std::vector<i32>& matToTex);
    static xMesh LoadMesh(VkPhysicalDevice physicalDevice, VkDevice device,
                          VkQueue transferQueue, VkCommandPool transferCommandPool,
                          const class aiMesh* mesh, const std::vector<i32>& matToTex);

    ~MeshModel() = default;
    void DestroyMesh();

    [[nodiscard]] size_t GetMeshCount() const;

    [[nodiscard]] const xMesh* GetMesh(size_t index) const;
    [[nodiscard]] m4& GetModel();

    void SetModel(const m4& model);
private:
    std::vector<xMesh> MeshList;
    m4 Model{};
};


#endif //X_MESH_MODEL_H
