//
// Created by Raul Romero on 2023-10-25.
//

#include "Navigation.h"
#include "util/Util.h"
#include <util/primitives.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>

namespace x::Navigation
{

void FindIncenter(const Triangle2D& triangle, v2& incenter)
{
    float a = glm::distance(triangle.vertices[1], triangle.vertices[2]);
    float b = glm::distance(triangle.vertices[0], triangle.vertices[2]);
    float c = glm::distance(triangle.vertices[0], triangle.vertices[1]);

    incenter = (a * triangle.vertices[0] + b * triangle.vertices[1] + c * triangle.vertices[2]) / (a + b + c);
}

void FindCircumcircle(const Triangle2D &triangle, glm::vec2 &circumcenter, float &circumradius)
{
    v2 A = triangle.vertices[0];
    v2 B = triangle.vertices[1];
    v2 C = triangle.vertices[2];

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

std::vector<TriangleNode> BowyerWatson(std::vector<v2>& points)
{
    TriangleNode supraTriangle = TriangleNode(Triangle2D({-1000.0f, -1000.0f}, {1000.0f, -1000.0f}, {0.0f, 1000.0f}));

    std::vector<TriangleNode> graphTriangles = {supraTriangle};
    for (v2& point : points)
    {
        std::vector<TriangleNode> badTriangles = {};
        for (TriangleNode& triangle : graphTriangles)
        {
            v2 circumCenter;
            f32 circumRadius;
            FindCircumcircle(triangle.GetTriangle(), circumCenter, circumRadius);
            if(glm::distance(point, circumCenter) < circumRadius)
            {
                badTriangles.push_back(triangle);
            }
        }

        std::vector<Edge2D> polygon = {};
        for (int i = 0; i < badTriangles.size(); i++)
        {
            const Triangle2D& triangle = badTriangles[i].GetTriangle();
            for (const Edge2D& edge : triangle.edges)
            {
                bool rejectEdge = false;
                for (int t = 0; t < badTriangles.size(); t++)
                {
                    if (t != i && x::Util::ContainsEdge(badTriangles[t].GetTriangle(), edge))
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

        graphTriangles.erase(std::remove_if(graphTriangles.begin(), graphTriangles.end(), [&](const TriangleNode& t) {
            return std::find(badTriangles.begin(), badTriangles.end(), t) != badTriangles.end();
        }), graphTriangles.end());

        for(const Edge2D& edge : polygon)
        {
            TriangleNode graphTriangle = TriangleNode(Triangle2D(edge.vertices[0], edge.vertices[1], point));
            graphTriangles.emplace_back(graphTriangle);
        }
    }

    graphTriangles.erase(std::remove_if(graphTriangles.begin(), graphTriangles.end(), [&](const TriangleNode& t)
    {
        return t.GetTriangle().vertices[0] == supraTriangle.GetTriangle().vertices[0] ||
               t.GetTriangle().vertices[0] == supraTriangle.GetTriangle().vertices[1] ||
               t.GetTriangle().vertices[0] == supraTriangle.GetTriangle().vertices[2] ||
               t.GetTriangle().vertices[1] == supraTriangle.GetTriangle().vertices[0] ||
               t.GetTriangle().vertices[1] == supraTriangle.GetTriangle().vertices[1] ||
               t.GetTriangle().vertices[1] == supraTriangle.GetTriangle().vertices[2] ||
               t.GetTriangle().vertices[2] == supraTriangle.GetTriangle().vertices[0] ||
               t.GetTriangle().vertices[2] == supraTriangle.GetTriangle().vertices[1] ||
               t.GetTriangle().vertices[2] == supraTriangle.GetTriangle().vertices[2];
    }), graphTriangles.end());

    u32 idx = 0;
    for(TriangleNode& graphTriangle : graphTriangles)
    {
        graphTriangle.SetIndex(idx++);
        const Triangle2D& t = graphTriangle.GetTriangle();
        for(TriangleNode& neighbor : graphTriangles)
        {
            for(const Edge2D& edge : t.edges)
            {
                if(x::Util::ContainsEdge(neighbor.GetTriangle(), edge))
                {
                    if(graphTriangle != neighbor)
                    {
                        graphTriangle.AddNeighbor(&neighbor);
                    }
                }
            }
        }
    }
    return graphTriangles;
}

bool PointInTriangle(const v2 &p, const Triangle2D &triangle)
{
    const v2* v = triangle.vertices;

    f32 s = (v[0].x - v[2].x) * (p.y - v[2].y) - (v[0].y - v[2].y) * (p.x - v[2].x);
    f32 t = (v[1].x - v[0].x) * (p.y - v[0].y) - (v[1].y - v[0].y) * (p.x - v[0].x);

    if ((s < 0) != (t < 0) && s != 0 && t != 0)
        return false;

    f32 d = (v[2].x - v[1].x) * (p.y - v[1].y) - (v[2].y - v[1].y) * (p.x - v[1].x);
    return d == 0 || (d < 0) == (s + t <= 0);
}

std::vector<TriangleNode*> ReconstructPath(TriangleNode* end, TriangleNode* start)
{
    std::vector<TriangleNode*> path;
    TriangleNode* current = end;
    while(current != start)
    {
        path.push_back(current);
        current = current->GetParent();
    }
    return path;
}

void AStar(const v2 &start, const v2 &end, std::vector<TriangleNode*> &path, std::vector<v2>& points)
{
    TriangleNode* startTriangle = nullptr;
    TriangleNode* endTriangle = nullptr;
    std::set<TriangleNode*> open;
    std::set<TriangleNode*> closed;

    std::vector<TriangleNode> graphTriangles = BowyerWatson(points);

    for(TriangleNode& graphTriangle : graphTriangles)
    {
        if(x::Navigation::PointInTriangle(start, graphTriangle.GetTriangle()))
        {
            startTriangle = &graphTriangle;
        }
        if(x::Navigation::PointInTriangle(end, graphTriangle.GetTriangle()))
        {
            endTriangle = &graphTriangle;
        }
    }

    if(startTriangle == nullptr || endTriangle == nullptr)
    {
        return;
    }

    open.emplace(startTriangle);

    while(!open.empty())
    {
        TriangleNode *current = *open.begin();

        for (TriangleNode *node : open)
        {
            if (node->GetFCost() < current->GetFCost() || node->GetFCost() == current->GetFCost() && node->GetHCost() < current->GetHCost())
            {
                current = node;
            }
        }

        open.erase(open.find(current));
        closed.emplace(current);

        if(current == endTriangle)
        {
            break;
        }

        for(TriangleNode* neighbor : current->GetNeighbors())
        {
            if(closed.find(neighbor) != closed.end())
            {
                continue;
            }

            f32 newMovementCostToNeighbor = current->GetGCost() + glm::distance(current->GetCenter(), neighbor->GetCenter());
            if(newMovementCostToNeighbor < neighbor->GetGCost() || open.find(neighbor) == open.end())
            {
                neighbor->SetGCost(newMovementCostToNeighbor);
                neighbor->SetHCost(glm::distance(neighbor->GetCenter(), endTriangle->GetCenter()));
                neighbor->SetParent(current);
                open.emplace(neighbor);
            }
        }
    }

    path = ReconstructPath(endTriangle, startTriangle);
}

void TriangleNode::AddNeighbor(TriangleNode *neighbor)
{
    if(std::find(neighbors.begin(), neighbors.end(), neighbor) != neighbors.end() && *neighbors.end() == neighbor)
        return;

    if(std::find(neighbor->neighbors.begin(), neighbor->neighbors.end(), this) != neighbor->neighbors.end() && *neighbor->neighbors.end() == this)
        return;

    neighbors.push_back(neighbor);
}

void TriangleNode::RemoveNeighbor(TriangleNode *neighbor)
{
    neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), neighbor), neighbors.end());
}

bool TriangleNode::operator==(const TriangleNode &other) const
{
    return GetTriangle() == other.GetTriangle();
}

bool TriangleNode::operator!=(const TriangleNode &other) const
{
    return !(*this == other);
}

void TriangleNode::SetIndex(u32 i)
{
    Index = i;
}

TriangleNode::TriangleNode(const Triangle2D &triangle, u32 index): triangle(triangle), Index(index), circumcenter(v2(0.f)), circumradius(0.f), incenter(v2(0.f))
{
    FindCircumcircle(triangle, circumcenter, circumradius);
    FindIncenter(triangle, incenter);
}

}