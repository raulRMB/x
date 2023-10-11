//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_TEST_SCENE_H
#define X_TEST_SCENE_H

#include "../core/Scene.h"

class TestScene final : public x::Scene
{
public:
    void Start() override;
    void HandleInput(const SDL_Event& event) override;
    void Update(f32 deltaTime) override;
    void Clean() override;
};


#endif //X_TEST_SCENE_H
