//
// Created by Raul Romero on 2023-10-11.
//

#include "MainScene.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/PhysicsComponent.h"
#include "Components/TargetComponent.h"
#include "engine/engine.h"
#include "core/Camera.h"
#include "Components/ParentComponent.h"
#include "engine/NetworkDriver.h"

void MainScene::Start()
{

}

void MainScene::Update(f32 deltaTime)
{
}

void MainScene::Clean()
{
    for(entt::entity entity : Entities)
    {
        RemoveEntity(entity);
    }
}

void MainScene::HandleClientInput(const i32 NetId)
{

}
