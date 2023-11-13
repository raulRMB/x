#include "SkeletalMesh.h"
#include "MeshModel.h"
#include <glm/gtx/quaternion.hpp>
#include <utility>
#include <iostream>
#include "../Util/Color.h"

SkeletalMesh::SkeletalMesh(const std::vector<SkeletalVertex> &vertices, const std::vector<u32> &indices,
                           u32 textureId, VkQueue transferQueue, VkCommandPool transferCommandPool,
                           VkPhysicalDevice physicalDevice, VkDevice device, SkeletalAnimation animation, u32 boneCount,
                           Bone rootBone, m4 globalInverseTransform) :
        Vertices(vertices),
        Indices(indices),
        TextureId(textureId),
        PhysicalDevice(physicalDevice),
        Device(device),
        Animation(std::move(animation)),
        BoneCount(boneCount),
        RootBone(std::move(rootBone)),
        GlobalInverseTransform(globalInverseTransform)
{
    VertexBuffer = CreateVertexBuffer(transferQueue, transferCommandPool);
    IndexBuffer = CreateIndexBuffer(transferQueue, transferCommandPool);
    CurrentPose.resize(BoneCount, m4(1.0f));
}

void SkeletalMesh::CalculateGlobalInverseTransform(const aiScene *scene)
{
    GlobalInverseTransform = AssimpToGlmMat4(scene->mRootNode->mTransformation);
    GlobalInverseTransform = glm::inverse(GlobalInverseTransform);
}

SkeletalAnimation SkeletalMesh::LoadAnimation(const aiScene *scene)
{
    aiAnimation* anim = scene->mAnimations[0];
    SkeletalAnimation animation = {};

    if(anim->mTicksPerSecond != 0.0f)
        animation.TicksPerSecond = (f32)anim->mTicksPerSecond;
    else
        animation.TicksPerSecond = 1.0f;

    animation.Duration = (f32)anim->mDuration / animation.TicksPerSecond;
    animation.BoneTransformTracks = {};

    for(i32 i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        BoneTransformTrack track = {};
        for(i32 j = 0; j < channel->mNumPositionKeys; j++)
        {
            track.PositionTimeStamps.push_back((f32)channel->mPositionKeys[j].mTime / animation.TicksPerSecond);
            track.Positions.push_back(AssimpToGlmVec3(channel->mPositionKeys[j].mValue));
        }
        for(i32 j = 0; j < channel->mNumRotationKeys; j++)
        {
            track.RotationTimeStamps.push_back((f32)channel->mRotationKeys[j].mTime / animation.TicksPerSecond);
            track.Rotations.push_back(AssimpToGlmQuat(channel->mRotationKeys[j].mValue));
        }
        for(i32 j = 0; j < channel->mNumScalingKeys; j++)
        {
            track.ScaleTimeStamps.push_back((f32)channel->mScalingKeys[j].mTime / animation.TicksPerSecond);
            track.Scales.push_back(AssimpToGlmVec3(channel->mScalingKeys[j].mValue));
        }
        animation.BoneTransformTracks[channel->mNodeName.C_Str()] = track;
    }

    return animation;
}

SkeletalMesh SkeletalMesh::LoadMesh(VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transferQueue,
                                    VkCommandPool transferCommandPool, const aiScene *scene, aiMesh *mesh, u32 textureId)
{
    std::vector<SkeletalVertex> vertices = {};
    std::vector<u32> indices = {};
    u32 boneCount = 0;
    Bone rootBone = {};

    for(u32 i = 0; i < mesh->mNumVertices; i++)
    {
        SkeletalVertex vertex = {};
        vertex.Pos.x = mesh->mVertices[i].x;
        vertex.Pos.y = mesh->mVertices[i].y;
        vertex.Pos.z = mesh->mVertices[i].z;
        vertex.Tex.x = mesh->mTextureCoords[0][i].x;
        vertex.Tex.y = mesh->mTextureCoords[0][i].y;
        vertex.Col = x::Color::White;
        vertex.BoneIds = iv4(0);
        vertex.BoneWeights = v4(0.0f);
        vertices.push_back(vertex);
    }

    std::unordered_map<std::string, std::pair<i32, m4>> boneInfo = {};
    std::vector<u32> boneCounts = {};
    boneCounts.resize(vertices.size(), 0);
    boneCount = mesh->mNumBones;

    for(i32 i = 0; i < boneCount; i++)
    {
        aiBone* bone = mesh->mBones[i];
        m4 m = AssimpToGlmMat4(bone->mOffsetMatrix);
        boneInfo[bone->mName.C_Str()] = std::make_pair(i, m);

        for(i32 j = 0; j < bone->mNumWeights; j++)
        {
            u32 id = bone->mWeights[j].mVertexId;
            f32 weight = bone->mWeights[j].mWeight;
            boneCounts[id]++;
            if(boneCounts[id] < 4)
            {
                switch (boneCounts[id])
                {
                case 1:
                    vertices[id].BoneIds.x = i;
                    vertices[id].BoneWeights.x = weight;
                    break;
                case 2:
                    vertices[id].BoneIds.y = i;
                    vertices[id].BoneWeights.y = weight;
                    break;
                case 3:
                    vertices[id].BoneIds.z = i;
                    vertices[id].BoneWeights.z = weight;
                    break;
                case 4:
                    vertices[id].BoneIds.w = i;
                    vertices[id].BoneWeights.w = weight;
                    break;
                }
            }
        }
    }

    for(SkeletalVertex& vertex : vertices)
    {
        f32 totalWeight = 0.0f;
        for(i32 i = 0; i < 4; i++)
        {
            totalWeight += vertex.BoneWeights[i];
        }
        if(totalWeight > 0.0f)
        {
            for(i32 i = 0; i < 4; i++)
            {
                vertex.BoneWeights[i] /= totalWeight;
            }
        }
    }

    for(u32 i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];
        for(u32 j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    ReadSkeleton(rootBone, scene->mRootNode, boneInfo);
    SkeletalAnimation animation = LoadAnimation(scene);

    m4 globalInverseTransform = AssimpToGlmMat4(scene->mRootNode->mTransformation);
    globalInverseTransform = glm::inverse(globalInverseTransform);

    SkeletalMesh skeletalMesh = SkeletalMesh(vertices, indices, textureId, transferQueue, transferCommandPool, physicalDevice, device, animation, boneCount, rootBone, globalInverseTransform);
    skeletalMesh.CalculateGlobalInverseTransform(scene);

    return skeletalMesh;
}

bool SkeletalMesh::ReadSkeleton(Bone &bone, aiNode *node, std::unordered_map<std::string, std::pair<i32, m4>> &boneInfoTable)
{
    if(boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
    {
        bone.Name = node->mName.C_Str();
        bone.Id = boneInfoTable[bone.Name].first;
        bone.OffsetMatrix = boneInfoTable[bone.Name].second;

        for(i32 i = 0; i < node->mNumChildren; i++)
        {
            Bone child;
            ReadSkeleton(child, node->mChildren[i], boneInfoTable);
            bone.Children.push_back(child);
        }
        return true;
    }
    else
    {
        for(i32 i = 0; i < node->mNumChildren; i++)
        {
            if(ReadSkeleton(bone, node->mChildren[i], boneInfoTable))
                return true;
        }
    }
    return false;
}

VkBuffer SkeletalMesh::CreateVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool)
{
    VkDeviceSize bufferSize = sizeof(SkeletalVertex) * Vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, Vertices.data(), bufferSize);
    vkUnmapMemory(Device, stagingBufferMemory);

    CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);

    CopyBuffer(Device, transferQueue, transferCommandPool, stagingBuffer, VertexBuffer, bufferSize);

    vkDestroyBuffer(Device, stagingBuffer, nullptr);
    vkFreeMemory(Device, stagingBufferMemory, nullptr);

    return VertexBuffer;
}

VkBuffer SkeletalMesh::CreateIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool)
{
    VkDeviceSize bufferSize = sizeof(u32) * Indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, Indices.data(), bufferSize);
    vkUnmapMemory(Device, stagingBufferMemory);

    CreateBuffer(PhysicalDevice, Device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory);

    CopyBuffer(Device, transferQueue, transferCommandPool, stagingBuffer, IndexBuffer, bufferSize);

    vkDestroyBuffer(Device, stagingBuffer, nullptr);
    vkFreeMemory(Device, stagingBufferMemory, nullptr);

    return IndexBuffer;
}

std::pair<u32, f32> SkeletalMesh::GetTimeFraction(const std::vector<f32>& timeStamps, f32& animationTime)
{
    u32 segment = 0;
    while(animationTime > timeStamps[segment])
    {
        segment++;
    }
    if(segment == timeStamps.size() - 1)
        return std::make_pair(segment, 0.0f);

    f32 start = timeStamps[segment - 1];
    f32 end = timeStamps[segment];
    f32 fraction = (animationTime - start) / (end - start);
    return std::make_pair(segment, fraction);
}

void SkeletalMesh::GetPose(SkeletalAnimation &animation, const Bone &skeleton, f32 animationTime, std::vector<m4> &outPose,
                           const m4 &parentTransform, const m4 &globalInverseTransform)
{
    const BoneTransformTrack &track = animation.BoneTransformTracks[skeleton.Name];

    animationTime = fmod(animationTime, animation.Duration);
    v3 position = v3(0.f);
    v3 scale = v3(1.f);
    q4 rotation = glm::identity<q4>();
    std::pair<u32, f32> timeFraction;
    if (!track.Positions.empty())
    {
        timeFraction = GetTimeFraction(track.PositionTimeStamps, animationTime);
        if (timeFraction.first == track.PositionTimeStamps.size() - 1)
            timeFraction.first = 0;
        position = glm::mix(track.Positions[timeFraction.first], track.Positions[timeFraction.first + 1],
                            timeFraction.second);
    }

    if (!track.Rotations.empty())
    {
        timeFraction = GetTimeFraction(track.RotationTimeStamps, animationTime);
        if (timeFraction.first == track.RotationTimeStamps.size() - 1)
            timeFraction.first = 0;
        rotation = glm::slerp(track.Rotations[timeFraction.first], track.Rotations[timeFraction.first + 1],
                              timeFraction.second);
    }

    if (!track.Scales.empty())
    {
        timeFraction = GetTimeFraction(track.ScaleTimeStamps, animationTime);
        if (timeFraction.first == track.ScaleTimeStamps.size() - 1)
            timeFraction.first = 0;
        scale = glm::mix(track.Scales[timeFraction.first], track.Scales[timeFraction.first + 1], timeFraction.second);
    }

    m4 localTransform = glm::translate(m4(1.0f), position) * glm::toMat4(rotation) * glm::scale(m4(1.0f), scale);
    m4 globalTransform = parentTransform * localTransform;

    outPose[skeleton.Id] = globalInverseTransform * globalTransform * skeleton.OffsetMatrix;

    for(const Bone& bone : skeleton.Children)
    {
        if(bone.Name.empty())
        {
            continue;
        }
        GetPose(animation, bone, animationTime, outPose, globalTransform, globalInverseTransform);
    }
}
