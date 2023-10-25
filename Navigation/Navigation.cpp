//
// Created by Raul Romero on 2023-10-25.
//

#include "Navigation.h"
#include "util/Util.h"
#include <util/primitives.h>
#include <vector>
#include <algorithm>

namespace x::Navigation
{

void FindCircumcircle(const Triangle2D &triangle, glm::vec2 &circumcenter, float &circumradius)
{
    glm::vec2 A = triangle.vertices[0];
    glm::vec2 B = triangle.vertices[1];
    glm::vec2 C = triangle.vertices[2];

    float D = 2.0f * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));
    if (std::abs(D) < glm::epsilon<float>()) {
        circumcenter = glm::vec2(0.0f);
        circumradius = 0.0f;
        return;
    }

    circumcenter.x = (1.f/D) * ((A.x * A.x + A.y * A.y) * (B.y - C.y) +
                                (B.x * B.x + B.y * B.y) * (C.y - A.y) +
                                (C.x * C.x + C.y * C.y) * (A.y - B.y));
    circumcenter.y = (1.f/D) * ((A.x * A.x + A.y * A.y) * (C.x - B.x) +
                                (B.x * B.x + B.y * B.y) * (A.x - C.x) +
                                (C.x * C.x + C.y * C.y) * (B.x - A.x));

    circumradius = glm::distance(A, circumcenter);
}

std::vector<Triangle2D> BowyerWatson(std::vector<v2>& points)
{
    Triangle2D supraTriangle = Triangle2D({-1000.0f, -1000.0f}, {1000.0f, -1000.0f}, {0.0f, 1000.0f});

    std::vector<Triangle2D> triangles = {supraTriangle};
    for (v2& p : points)
    {
        std::vector<Triangle2D> badTriangles = {};
        for (Triangle2D& triangle : triangles)
        {
            v2 circumCenter;
            f32 circumRadius;
            FindCircumcircle(triangle, circumCenter, circumRadius);
            if(glm::distance(p, circumCenter) < circumRadius)
            {
                badTriangles.push_back(triangle);
            }
        }

        std::vector<Edge2D> polygon = {};
        for (int i = 0; i < badTriangles.size(); i++)
        {
            Triangle2D& triangle = badTriangles[i];
            for (const Edge2D& edge : triangle.edges)
            {
                bool rejectEdge = false;
                for (int t = 0; t < badTriangles.size(); t++)
                {
                    if (t != i && x::Util::ContainsEdge(badTriangles[t], edge))
                    {
                        rejectEdge = true;
                        break;
                    }
                }
                if (!rejectEdge){
                    polygon.push_back(edge);
                }
            }
        }

        triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&](const Triangle2D& t) {
            return std::find(badTriangles.begin(), badTriangles.end(), t) != badTriangles.end();
        }), triangles.end());

        for(const Edge2D& edge : polygon)
        {
            triangles.emplace_back(edge.vertices[0], edge.vertices[1], p);
        }
    }

    triangles.erase(std::remove_if(triangles.begin(), triangles.end(), [&](const Triangle2D& t)
    {
        return t.vertices[0] == supraTriangle.vertices[0] ||
               t.vertices[0] == supraTriangle.vertices[1] ||
               t.vertices[0] == supraTriangle.vertices[2] ||
               t.vertices[1] == supraTriangle.vertices[0] ||
               t.vertices[1] == supraTriangle.vertices[1] ||
               t.vertices[1] == supraTriangle.vertices[2] ||
               t.vertices[2] == supraTriangle.vertices[0] ||
               t.vertices[2] == supraTriangle.vertices[1] ||
               t.vertices[2] == supraTriangle.vertices[2];
    }), triangles.end());

    return triangles;
}

}