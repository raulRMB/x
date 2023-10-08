//
// Created by Raul Romero on 2023-10-07.
//

#ifndef X_PRIMITIVES_H
#define X_PRIMITIVES_H

#include <glm/glm.hpp>
#include "util/color.h"

namespace X::Primitives2D
{
    struct Square
    {
        static glm::vec2 TopLeft() { return {-1.0f, -1.0f}; }
        static glm::vec2 TopRight() { return {1.0f, -1.0f}; }
        static glm::vec2 BottomRight() { return {1.0f, 1.0f}; }
        static glm::vec2 BottomLeft() { return {-1.0f, 1.0f}; }
    };

    std::vector<xRUtil::Vertex> MakeSquare(const glm::vec4& color)
    {
        std::vector<xRUtil::Vertex> vertices =
        {
                {{Square::TopLeft(), 0.}, color, {.0, 0.}},
                {{Square::TopRight(), 0.}, color, {1., 0.}},
                {{Square::BottomRight(), 0.}, color, {1., 1.}},
                {{Square::BottomLeft(), 0.}, color, {0., 1.}}
        };

        return vertices;
    }

    struct Triangle
    {
        static glm::vec2 Top() { return {0.0f, -1.0f}; }
        static glm::vec2 Right() { return {1.0f, 1.0f}; }
        static glm::vec2 Left() { return {-1.0f, 1.0f}; }
    };

    std::vector<xRUtil::Vertex> MakeTriangle(const glm::vec4& color)
    {
        std::vector<xRUtil::Vertex> vertices =
        {
                {{Triangle::Top(), 0.}, color, {.5, 0.}},
                {{Triangle::Right(), 0.}, color, {1., 1.}},
                {{Triangle::Left(), 0.}, color, {0., 1.}}
        };

        return vertices;
    }

    struct Circle
    {
        static glm::vec2 Center() { return {0.0f, 0.0f}; }
        static float Radius() { return 1.0f; }
    };

    std::vector<xRUtil::Vertex> MakeCircle(const glm::vec4& color, u32 segments)
    {
        std::vector<xRUtil::Vertex> vertices;
        vertices.reserve(segments + 1);
        vertices.push_back({glm::vec3(Circle::Center(), 0.f), color, {0.5f, 0.5f}});

        float angle = 0.0f;
        float angleIncrement = 360.0f / (f32)segments;
        for (u32 i = 0; i < segments; ++i)
        {
            glm::vec2 vertex = {Circle::Center().x + cos(glm::radians(angle)) * Circle::Radius(),
                                Circle::Center().y + sin(glm::radians(angle)) * Circle::Radius()};
            vertices.push_back({glm::vec3(vertex, 0.f), color, {0.5f, 0.5f}});
            angle += angleIncrement;
        }

        return vertices;
    }
}

#endif //X_PRIMITIVES_H
