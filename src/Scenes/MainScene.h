#ifndef X_MAIN_SCENE_H
#define X_MAIN_SCENE_H

#include "../core/Scene.h"
#include <entt.hpp>
#include "SDL2/SDL_events.h"
#include "../util/primitives.h"
#include "../Navigation/Navigation.h"
#include "../core/SkeletalMesh.h"

class MainScene final : public x::Scene
{
    std::vector<entt::entity> FollowEntities;

    v2 StartPoint;
    v2 EndPoint;

    std::vector<v2> points;

    std::vector<Edge2D> Portals = {};
    std::vector<TriangleNode> Tris = {};

    Bone Skeleton = {};
    m4 GlobalInverseTransform = m4(1.0f);
public:
    void Start() override;
    void Update(f32 deltaTime) override;
    void Clean() override;
    void HandleInput(const SDL_Event& event) override;
    void Save() override;
    void Load() override;
    void DrawUI() override;
};


#endif //X_MAIN_SCENE_H
