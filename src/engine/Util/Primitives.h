#ifndef X_PRIMITIVES_H
#define X_PRIMITIVES_H

#include <glm/glm.hpp>
#include <vector>
#include "../Util/Color.h"
#include "../Renderer/RendererUtil.h"

namespace x::Primitives2D
{
    struct Square
    {
        static v2 TopLeft() { return {-1.0f, -1.0f}; }
        static v2 TopRight() { return {1.0f, -1.0f}; }
        static v2 BottomRight() { return {1.0f, 1.0f}; }
        static v2 BottomLeft() { return {-1.0f, 1.0f}; }
    };

    inline std::vector<struct x::RenderUtil::Vertex> MakeSquare(const glm::vec4& color)
    {
        std::vector<x::RenderUtil::Vertex> vertices =
        {
                {{Square::TopLeft(), 0.}, color, {.0, 0.}},
                {{Square::TopRight(), 0.}, color, {1., 0.}},
                {{Square::BottomRight(), 0.}, color, {1., 1.}},
                {{Square::BottomLeft(), 0.}, color, {0., 1.}}
        };

        return vertices;
    }

    struct Edge {
        v2 vertices[2]{};
        struct Triangle* triangle;
    };

    struct Triangle {

        v2 vertices[3]{};
        Edge edges[3]{}; // edges[i] = {vertices[i], vertices[(i + 1) % 3]}

        Triangle(v2 A, v2 B, v2 C) {
            vertices[0] = A;
            vertices[1] = B;
            vertices[2] = C;

            edges[0].vertices[0] = A;
            edges[0].vertices[1] = B;
            edges[1].vertices[0] = B;
            edges[1].vertices[1] = C;
            edges[2].vertices[0] = C;
            edges[2].vertices[1] = A;
        }

        bool operator==(const Triangle& other) const {
            return vertices[0] == other.vertices[0] && vertices[1] == other.vertices[1] && vertices[2] == other.vertices[2];
        }
        bool operator!=(const Triangle& other) const {
            return !(*this == other);
        }
    };

    struct Circle
    {
        static v2 Center() { return {0.0f, 0.0f}; }
        static float Radius() { return 1.0f; }
    };

    inline std::vector<x::RenderUtil::Vertex> MakeFilledCircle(const glm::vec4& color, u32 segments)
    {
        std::vector<x::RenderUtil::Vertex> vertices;
        vertices.reserve(segments + 1);
        vertices.push_back({v3(Circle::Center(), 0.f), color, {0.5f, 0.5f}});

        float angle = 0.0f;
        float angleIncrement = 360.0f / (f32)segments;
        for (u32 i = 0; i < segments; ++i)
        {
            v2 vertex = {Circle::Center().x + cos(glm::radians(angle)) * Circle::Radius(),
                                Circle::Center().y + sin(glm::radians(angle)) * Circle::Radius()};
            vertices.push_back({v3(vertex, 0.f), color, {0.5f, 0.5f}});
            angle += angleIncrement;
        }

        return vertices;
    }

    inline std::vector<x::RenderUtil::Vertex> MakeCircle(const glm::vec4& color, u32 segments)
    {
        std::vector<x::RenderUtil::Vertex> vertices;
        vertices.reserve(segments + 1);
        float angle = 0.0f;
        float angleIncrement = 360.0f / (f32)segments;
        for (u32 i = 0; i < segments; ++i)
        {
            v2 vertex = {Circle::Center().x + cos(glm::radians(angle)) * Circle::Radius(),
                                Circle::Center().y + sin(glm::radians(angle)) * Circle::Radius()};
            vertices.push_back({v3(vertex, 0.f), color, {0.5f, 0.5f}});
            angle += angleIncrement;
        }

        return vertices;
    }

    enum class Shape
    {
        Square,
        Triangle,
        Circle
    };
}

typedef x::Primitives2D::Square Square2D;
typedef x::Primitives2D::Circle Circle2D;
typedef x::Primitives2D::Triangle Triangle2D;
typedef x::Primitives2D::Edge Edge2D;


#endif //X_PRIMITIVES_H
