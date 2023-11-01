//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_MAIN_SCENE_H
#define X_MAIN_SCENE_H

#include "../core/Scene.h"
#include "../vendor/entt.hpp"
#include "SDL2/SDL_events.h"
#include "util/primitives.h"
#include "Navigation/Navigation.h"

class MainScene final : public x::Scene
{
    std::vector<entt::entity> FollowEntities;

    v2 StartPoint;
    v2 EndPoint;

    std::vector<v2> points;

    std::vector<Edge2D> Portals = {};
    std::vector<TriangleNode> Tris = {};

public:
    void Start() override;
    void Update(f32 deltaTime) override;
    void Clean() override;
    void HandleInput(const SDL_Event& event) override;
    void Save() override;
    void Load() override;
};


#endif //X_MAIN_SCENE_H
