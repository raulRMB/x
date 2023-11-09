#ifndef X_SKELETAL_MESH_H
#define X_SKELETAL_MESH_H

#include "../engine/rutil.h"
#include "assimp/matrix4x4.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "MeshModel.h"

struct Bone
{
    i32 Id = 0;
    std::string Name;
    m4 OffsetMatrix = m4(1.0f);
    std::vector<Bone> Children = {};
};

struct BoneTransformTrack
{
    std::vector<f32> PositionTimeStamps = {};
    std::vector<f32> RotationTimeStamps = {};
    std::vector<f32> ScaleTimeStamps = {};

    std::vector<v3> Positions = {};
    std::vector<q4> Rotations = {};
    std::vector<v3> Scales = {};
};

struct SkeletalAnimation
{
    f32 Duration = 0.f;
    f32 TicksPerSecond = 0.f;
    std::unordered_map<std::string, BoneTransformTrack> BoneTransformTracks = {};
};

struct BoneTransforms
{
    std::array<m4, 100> Bones = {};
};

inline v3 AssimpToGlmVec3(const aiVector3D& vector)
{
    return {vector.x, vector.y, vector.z};
}

inline q4 AssimpToGlmQuat(const aiQuaternion& quat)
{
    return {quat.w, quat.x, quat.y, quat.z};
}

inline v2 AssimpToGlmVec2(const aiVector2D& vector)
{
    return {vector.x, vector.y};
}

inline m4 AssimpToGlmMat4(const aiMatrix4x4& matrix)
{
    m4 result;
    result[0][0] = matrix.a1; result[0][1] = matrix.b1; result[0][2] = matrix.c1; result[0][3] = matrix.d1;
    result[1][0] = matrix.a2; result[1][1] = matrix.b2; result[1][2] = matrix.c2; result[1][3] = matrix.d2;
    result[2][0] = matrix.a3; result[2][1] = matrix.b3; result[2][2] = matrix.c3; result[2][3] = matrix.d3;
    result[3][0] = matrix.a4; result[3][1] = matrix.b4; result[3][2] = matrix.c4; result[3][3] = matrix.d4;
    return result;
}

class SkeletalMesh
{
    std::vector<x::RenderUtil::SkeletalVertex> Vertices = {};
    std::vector<u32> Indices = {};
    u32 BoneCount = 0;
    SkeletalAnimation Animation = {};
    Bone RootBone = {};

    u32 TextureId = 0;

    m4 GlobalInverseTransform = m4(1.0f);

    std::vector<m4> CurrentPose = {};

    VkBuffer VertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory VertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer IndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory IndexBufferMemory = VK_NULL_HANDLE;
    VkDevice Device = VK_NULL_HANDLE;
    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;

    void CalculateGlobalInverseTransform(const aiScene* scene);

    static SkeletalAnimation LoadAnimation(const aiScene* scene);

    static bool ReadSkeleton(Bone& bone, aiNode* node, std::unordered_map<std::string, std::pair<i32, m4>>& boneInfoTable);

    SkeletalMesh() = default;

    SkeletalMesh(const std::vector<x::RenderUtil::SkeletalVertex>& vertices, const std::vector<u32>& indices,
                 u32 textureId, VkQueue transferQueue, VkCommandPool transferCommandPool,
                 VkPhysicalDevice physicalDevice, VkDevice device, SkeletalAnimation animation, u32 boneCount, Bone rootBone);

    VkBuffer CreateVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool);
    VkBuffer CreateIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool);

    static std::pair<u32, f32> GetTimeFraction(const std::vector<f32>& timeStamps, f32& animationTime);
public:
    static SkeletalMesh LoadMesh(VkPhysicalDevice physicalDevice, VkDevice device,
                                 VkQueue transferQueue, VkCommandPool transferCommandPool,
                                 const aiScene* scene, aiMesh* mesh, u32 textureId);
    inline VkBuffer GetVertexBuffer() const { return VertexBuffer; }
    inline VkBuffer GetIndexBuffer() const { return IndexBuffer; }
    inline u32 GetTextureId() const { return TextureId; }

    inline u32 GetIndexCount() const { return Indices.size(); }

    inline SkeletalAnimation& GetAnimation() { return Animation; }
    inline Bone& GetRootBone() { return RootBone; }
    inline std::vector<m4>& GetCurrentPose() { return CurrentPose; }

    inline u32 GetBoneCount() const { return BoneCount; }

    m4& GetGlobalInverseTransform() { return GlobalInverseTransform; }

    static void GetPose(SkeletalAnimation& animation, Bone& skeleton, f32 animationTime, std::vector<m4>& outPose, m4& parentTransform, m4& globalInverseTransform);
};


#endif //X_SKELETAL_MESH_H
