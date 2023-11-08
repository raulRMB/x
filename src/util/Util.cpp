//
// Created by Raul Romero on 2023-10-25.
//

#include "Util.h"
#include "../util/primitives.h"

namespace x::Util
{

v3 Intersect(v3 planeP, v3 planeN, v3 rayP, v3 rayD)
{
    f32 d = glm::dot(planeP, -planeN);
    f32 t = -(d + glm::dot(rayP, planeN)) / glm::dot(rayD, planeN);
    return rayP + t * rayD;
}

bool ContainsEdge(const Triangle2D &triangle, const Edge2D &edge)
{
    int sharedVerts = 0;
    for (const v2 &vertex: triangle.vertices)
    {
        if (vertex == edge.vertices[0] || vertex == edge.vertices[1])
        {
            sharedVerts++;
        }
    }
    return sharedVerts == 2;
}

}