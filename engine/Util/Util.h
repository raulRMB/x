#ifndef X_UTIL_H
#define X_UTIL_H

#include "../Core/defines.h"
#include "../Util/Primitives.h"

namespace Util {
v3 Intersect(v3 planeP, v3 planeN, v3 rayP, v3 rayD);
bool ContainsEdge(const Triangle2D& triangle, const Edge2D& edge);
}

#endif //X_UTIL_H
