//
// Created by Raul Romero on 2023-10-25.
//

#include "Navigation.h"
#include "util/Util.h"
#include "Components/MeshComponent.h"
#include "engine/engine.h"
#include <util/primitives.h>
#include <vector>
#include <algorithm>
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

std::vector<TriangleNode*> ReconstructPath(TriangleNode* end, TriangleNode* start, std::vector<Edge2D>& portals)
{
    std::vector<TriangleNode*> path;
    TriangleNode* current = end;
    while(current != start)
    {
        path.push_back(current);
        current = current->GetParent();
    }
    path.push_back(start);

    std::reverse(path.begin(), path.end());

    for(i32 i = 0; i < path.size() - 1; i++)
    {
        if(const Edge2D* SharedEdge = GetSharedEdge(path[i]->GetTriangle(), path[i + 1]->GetTriangle()); SharedEdge != nullptr)
        {
            if(IsOnRight(path[i]->GetCenter(), path[i + 1]->GetCenter(), SharedEdge->vertices[0]))
            {
                portals.push_back({SharedEdge->vertices[1], SharedEdge->vertices[0]});
            }
            else
            {
                portals.push_back(*SharedEdge);
            }
        }
    }

    return path;
}

void AStar(const v2 &start, const v2 &end, std::vector<TriangleNode*> &path, std::vector<Edge2D>& portals, std::vector<v2>& points)
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
            if(closed.find(neighbor) != closed.end() || neighbor->IsBlocked())
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

    path = ReconstructPath(endTriangle, startTriangle, portals);
}

void AStar(const v2 &start, const v2 &end, std::vector<TriangleNode*> &path, std::vector<Edge2D>& portals, std::vector<TriangleNode>& graphTriangles)
{
    TriangleNode* startTriangle = nullptr;
    TriangleNode* endTriangle = nullptr;
    std::set<TriangleNode*> open;
    std::set<TriangleNode*> closed;

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
            if(closed.find(neighbor) != closed.end() || neighbor->IsBlocked())
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

    path = ReconstructPath(endTriangle, startTriangle, portals);
}

f32 TriangleArea2(const v2 &A, const v2 &B, const v2 &C)
{
    const f32 ax = B.x - A.x;
    const f32 ay = B.y - A.y;
    const f32 bx = C.x - A.x;
    const f32 by = C.y - A.y;
    return bx*ay - ax*by;
}

struct CFunnelLeft
{
    v2 pos;

    void operator = (const v2& pos)
    {
        this->pos = pos;
    }
} funnelLeft;

struct CFunnelRight
{
    v2 pos;

    void operator = (const v2& pos)
    {
        this->pos = pos;
    }
} funnelRight;

struct CFunnelApex
{
    v2 pos;

    void operator = (const v2& pos)
    {
        this->pos = pos;
    }
} funnelApex;


std::vector<v2> StringPull(const std::vector<Edge2D> &portals, const v2 &start, const v2 &end)
{
    std::vector<v2> path;

    v2 portalApex = start;
    v2 portalLeft = portals[0].vertices[0];
    v2 portalRight = portals[0].vertices[1];

    const_cast<std::vector<Edge2D>&>(portals).push_back({end, end});
    int apexIndex = 0, leftIndex = 0, rightIndex = 0;

    funnelLeft = portalLeft;
    funnelRight = portalRight;
    funnelApex = portalApex;

    path.push_back(portalApex);

    for(i32 i = 1; i < portals.size(); ++i)
    {
        const v2& left = portals[i].vertices[0];
        const v2& right = portals[i].vertices[1];

        f32 area = TriangleArea2(portalApex, portalRight, right);
        if(area <= 0.0f)
        {
            area = TriangleArea2(portalApex, portalLeft, right);
            if(portalApex == portalRight ||
                    area > 0.0f)
            {
                portalRight = right;
                rightIndex = i;
            }
            else
            {
                path.push_back(portalLeft);
                apexIndex = leftIndex;
                portalApex = portalLeft;
                portalLeft = portalApex;
                portalRight = portalApex;
                leftIndex = apexIndex;
                rightIndex = apexIndex;
                i = apexIndex;
                continue;
            }
        }
        area = TriangleArea2(portalApex, portalLeft, left);
        if(area >= 0.0f)
        {
            area = TriangleArea2(portalApex, portalRight, left);
            if(portalApex == portalLeft ||
                    area < 0.0f)
            {
                portalLeft = left;
                leftIndex = i;
            }
            else
            {
                path.push_back(portalRight);
                apexIndex = rightIndex;
                portalApex = portalRight;
                portalLeft = portalApex;
                portalRight = portalApex;
                leftIndex = apexIndex;
                rightIndex = apexIndex;
                i = apexIndex;
                continue;
            }
        }
    }

    if(path[path.size() - 1] != end)
    {
        path.push_back(end);
    }

    return path;
}


const Edge2D* GetSharedEdge(const Triangle2D &t1, const Triangle2D &t2)
{
    for(const Edge2D& edge : t1.edges)
    {
        if(x::Util::ContainsEdge(t2, edge))
        {
            return &edge;
        }
    }
    return nullptr;
}

bool IsOnRight(const v2 &O, const v2 &A, const v2 &B)
{
    v2 a = glm::normalize(A - O);
    v2 b = glm::normalize(B - O);

    return a.x * -b.y + a.y * b.x > 0;
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

TriangleNode::TriangleNode(const Triangle2D &triangle, u32 index) :
    triangle(triangle), Index(index),
    circumcenter(v2(0.f)), circumradius(0.f),
    incenter(v2(0.f)), parent(nullptr), bBlocked(false)
{
    FindCircumcircle(triangle, circumcenter, circumradius);
    FindIncenter(triangle, incenter);
}

}