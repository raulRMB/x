#ifndef X_SKELETAL_MESH_COMPONENT_H
#define X_SKELETAL_MESH_COMPONENT_H

#include "../Core/defines.h"

struct CSkeletalMesh
{
    u32 Id;
    CSkeletalMesh() = default;
    CSkeletalMesh(u32 id) : Id(id) {}
};

#endif //X_SKELETAL_MESH_COMPONENT_H
