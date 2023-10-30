//
// Created by Raul Romero on 2023-10-25.
//

#ifndef X_NAVIGATION_H
#define X_NAVIGATION_H

#include "base/defines.h"
#include <glm/gtx/norm.hpp>
#include <vector>
#include <util/primitives.h>

namespace x::Navigation
{

class TriangleNode
{
    Triangle2D triangle;
    u32 Index;
    std::vector<TriangleNode*> neighbors;
    f32 gCost = 0.f;
    f32 hCost = 0.f;
    v2 circumcenter;
    f32 circumradius;
    v2 incenter;
    TriangleNode* parent = nullptr;

public:
    TriangleNode(const Triangle2D& triangle, u32 index = 0);
    ~TriangleNode() = default;

    void AddNeighbor(TriangleNode* neighbor);
    void RemoveNeighbor(TriangleNode* neighbor);
    [[nodiscard]] inline const std::vector<TriangleNode*>& GetNeighbors() const { return neighbors; }
    [[nodiscard]] inline const Triangle2D& GetTriangle() const { return triangle; }
    [[nodiscard]] inline const u32 GetIndex() const { return Index; }

    [[nodiscard]] inline const f32 GetFCost() const { return gCost + hCost; }
    [[nodiscard]] inline const f32 GetGCost() const { return gCost; }
    [[nodiscard]] inline const f32 GetHCost() const { return hCost; }

    [[nodiscard]] inline const v2& GetCenter() const { return circumcenter; }
    [[nodiscard]] inline const f32 GetRadius() const { return circumradius; }

    [[nodiscard]] inline TriangleNode* GetParent() const { return parent; }

    inline void SetGCost(f32 cost) { gCost = cost; }
    inline void SetHCost(f32 cost) { hCost = cost; }

    void SetParent(TriangleNode *pNode) { parent = pNode; }

    bool operator==(const TriangleNode& other) const;

    bool operator!=(const TriangleNode& other) const;
    void SetIndex(u32 i);
};

void FindIncenter(const Triangle2D& triangle, v2& incenter);
void FindCircumcircle(const Triangle2D& triangle, glm::vec2& circumcenter, float& circumradius);
std::vector<TriangleNode> BowyerWatson(std::vector<v2>& points);
bool PointInTriangle(const v2& p, const Triangle2D& t);
void AStar(const v2 &start, const v2 &end, std::vector<TriangleNode*> &path, std::vector<v2>& points);

}
typedef x::Navigation::TriangleNode TriangleNode;


#endif //X_NAVIGATION_H
