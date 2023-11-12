#ifndef X_MESHCOMPONENT_H
#define X_MESHCOMPONENT_H

#include "../Core/defines.h"

struct CLineMesh
{
    u32 Id;
    CLineMesh() = default;
    CLineMesh(u32 id) : Id(id) {}
};

struct CTriangleMesh
{
    u32 Id;
    CTriangleMesh() = default;
    CTriangleMesh(u32 id) : Id(id) {}
};

#endif //X_MESHCOMPONENT_H
