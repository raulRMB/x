//
// Created by Raul Romero on 2023-10-13.
//

#include "MeshModel.h"
#include "../util/primitives.h"
#include <assimp/scene.h>

MeshModel::MeshModel(): Model(1.0f) {}

size_t MeshModel::GetMeshCount() const
{
    return MeshList.size();
}

const xMesh *MeshModel::GetMesh(size_t index) const
{
    if(index >= MeshList.size())
        throw std::runtime_error("MeshModel::GetMesh: index out of bounds");

    return &MeshList[index];
}

m4& MeshModel::GetModel()
{
    return Model;
}

void MeshModel::SetModel(const m4 &model)
{
    Model = model;
}

void MeshModel::DestroyMesh()
{
    for(xMesh& mesh : MeshList)
        mesh.DestroyBuffers();
}

std::vector<std::string> MeshModel::LoadMaterials(const aiScene* scene)
{
    std::vector<std::string> textureList(scene->mNumMaterials);

    for(u32 i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material = scene->mMaterials[i];
        textureList[i] = "";
        if(material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            aiString path;
            if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
            {
                auto idx = std::string(path.data).rfind('\\');
                std::string texturePath = std::string(path.data).substr(idx + 1);
                textureList[i] = texturePath;
            }
        }
    }

    return textureList;
}

std::vector<xMesh> MeshModel::LoadNode(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transferQueue,
                                       VkCommandPool transferCommandPool, const struct aiNode *node,
                                       const struct aiScene *scene, const std::vector<i32> &matToTex)
{
    std::vector<xMesh> meshList;

    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        meshList.push_back(LoadMesh(physicalDevice, device, transferQueue, transferCommandPool,
                                    scene->mMeshes[node->mMeshes[i]], matToTex));
    }

    for(size_t i = 0; i < node->mNumChildren; i++)
    {
        std::vector<xMesh> newList = LoadNode(physicalDevice, device, transferQueue, transferCommandPool,
                                              node->mChildren[i], scene, matToTex);
        meshList.insert(meshList.end(), newList.begin(), newList.end());
    }

    return meshList;
}

xMesh MeshModel::LoadMesh(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transferQueue,
                          VkCommandPool transferCommandPool, const struct aiMesh *mesh,
                          const std::vector<i32> &matToTex)
{
    std::vector<x::RenderUtil::Vertex> vertices;
    std::vector<u32> indices;

    vertices.resize(mesh->mNumVertices);

    for(size_t i = 0; i < mesh->mNumVertices; i++)
    {
        vertices[i].Pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
//        vertices[i].Nrm = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        if(mesh->mTextureCoords[0])
            vertices[i].Tex = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        else
            vertices[i].Tex = {0.0f, 0.0f};

        vertices[i].Col = x::Color::White;
    }

    for(size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    xMesh newMesh(vertices, indices, matToTex[mesh->mMaterialIndex], transferQueue, transferCommandPool,
                  physicalDevice, device);

    return newMesh;
}

MeshModel::MeshModel(std::vector<xMesh> meshList) : Model(1.0f)
{
    MeshList = std::move(meshList);
}


#pragma clang diagnostic pop