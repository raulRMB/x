//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_TRANSFORM_COMPONENT_H
#define X_TRANSFORM_COMPONENT_H

#include <base/defines.h>

struct CTransform2d
{
    v2 Position;
    f32 Scale;
    f32 Rotation;
    f32 Elevation;
};

struct CTransform3d
{
    v3 Position;
    v3 Scale;
    v3 Rotation;
};

#endif //X_TRANSFORM_COMPONENT_H
