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
    void Save() override {};
    void Load() override {};
    void DrawUI() override {};
};


#endif //X_TEST_SCENE_H
