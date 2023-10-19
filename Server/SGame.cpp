//
// Created by Raul Romero on 2023-10-19.
//

#include "SGame.h"
#include "SMainScene.h"

SGame::SGame()
{

}

SGame::~SGame()
{

}

void SGame::Start()
{

}

void SGame::Update(f32 deltaTime)
{

}

void SGame::Clean()
{

}

entt::registry &SGame::GetRegistry()
{
    return pMainScene->GetRegistry();
}
