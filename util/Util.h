//
// Created by Raul Romero on 2023-10-25.
//

#ifndef X_UTIL_H
#define X_UTIL_H

#include "base/defines.h"
#include <util/primitives.h>

namespace x::Util
{

v3 Intersect(v3 planeP, v3 planeN, v3 rayP, v3 rayD);
bool ContainsEdge(const Triangle2D& triangle, const Edge2D& edge);

}



#endif //X_UTIL_H
