#ifndef X_NETCOMPID_H
#define X_NETCOMPID_H

#include "base/defines.h"

enum class ENetCompId : u32
{
    Transform2d = 0,
    NetTransform2d,
    Transform3d,
    NetTransform3d,
    Physics2d,
    Physics3d,
    Axes,
    Target,
    Mesh,
};

#endif //X_NETCOMPID_H
