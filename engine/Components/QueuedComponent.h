#ifndef X_QUEUED_COMPONENT_H
#define X_QUEUED_COMPONENT_H

#include <bitset>
#include "../Core/defines.h"

enum class EComponent : u32
{
    None = 0,
    LineMesh,
    TriangleMesh,
    ComponentCount,
};

struct CQueued
{
    std::bitset<128> Components;

    CQueued() = default;
};

#endif //X_QUEUED_COMPONENT_H
