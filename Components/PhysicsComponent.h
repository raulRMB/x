//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_PHYSICSCOMPONENT_H
#define X_PHYSICSCOMPONENT_H

#include "../base/defines.h"

struct CPhysics2d
{
    v2 Velocity;
    v2 Acceleration;
    f32 Mass;
};

struct CPhysics3d
{
    v3 Velocity;
    v3 Acceleration;
    f32 Mass;
};

#endif //X_PHYSICSCOMPONENT_H
