//
// Created by Raul Romero on 2023-10-19.
//

#include "SMainScene.h"
#include "Components/TransformComponent.h"
#include "Components/NetworkComponent.h"

void SMainScene::Start()
{
    entt::entity e = CreateEntity();
    CTransform3d transform{};
    transform.WorldPosition = {0.0f, 0.0f, 0.0f};
    transform.WorldRotation = {glm::radians(90.f), 0.0f, 0.0f};
    transform.WorldScale = v3(.1f);
    AddComponent(e, transform);
    AddComponent(e, CNetwork{(u32)e});
    Entities.push_back(e);
}

void SMainScene::Update(f32 deltaTime)
{

}

void SMainScene::Clean()
{

}
