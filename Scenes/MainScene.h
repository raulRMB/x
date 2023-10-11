//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_MAIN_SCENE_H
#define X_MAIN_SCENE_H

#include "../core/Scene.h"
#include "../vendor/entt.hpp"

class MainScene final : public x::Scene
{
    v3 pos = {0, 0, 2};

public:
    void Start() override;
    void Update(f32 deltaTime) override;
    void Clean() override;
};


#endif //X_MAIN_SCENE_H
