//
// Created by Raul Romero on 2023-10-19.
//

#ifndef X_S_MAIN_SCENE_H
#define X_S_MAIN_SCENE_H

#include "../core/Scene.h"
#include <SDL2/SDL_events.h>

class SMainScene : public x::Scene
{
public:
    void Start() override;
    void HandleInput(const SDL_Event& event) override {};
    void Update(f32 deltaTime) override;
    void Clean() override;
};


#endif //X_S_MAIN_SCENE_H
