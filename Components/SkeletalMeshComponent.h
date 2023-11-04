//
// Created by Raul Romero on 2023-11-03.
//

#ifndef X_SKELETAL_MESH_COMPONENT_H
#define X_SKELETAL_MESH_COMPONENT_H

#include <base/defines.h>

struct CSkeletalMesh
{
    u32 Id;
    CSkeletalMesh() = default;
    CSkeletalMesh(u32 id) : Id(id) {}
};

#endif //X_SKELETAL_MESH_COMPONENT_H
