//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_MAIN_SCENE_H
#define X_MAIN_SCENE_H

#include "../core/Scene.h"
#include "../vendor/entt.hpp"
#include "SDL2/SDL_events.h"

class MainScene final : public x::Scene
{
    entt::entity TargetEntity;
    entt::entity FollowEntity;
public:
    void Start() override;
    void Update(f32 deltaTime) override;
    void Clean() override;
    void HandleInput(const SDL_Event& event) override;
};


#endif //X_MAIN_SCENE_H
