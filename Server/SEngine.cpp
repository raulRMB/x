//
// Created by Raul Romero on 2023-10-18.
//

#include "SEngine.h"
#include "SNetworkDriver.h"
#include "Components/NetworkComponent.h"
#include "Components/TransformComponent.h"
#include "Components/PhysicsComponent.h"
#include <thread>
#include <iostream>
#include "SGame.h"

SEngine::SEngine() : bRunning(false), LastTime(), CurrentTime(), TotalTime(0.0f), DeltaTime(0.0f)
{
}

i32 SEngine::Run()
{
    LastTime = std::chrono::high_resolution_clock::now();

    if(Init() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    std::thread networkThread([=]()
    {
        while (bRunning)
        {
            CurrentTime = std::chrono::high_resolution_clock::now();
            DeltaTime = CurrentTime - LastTime;
            LastTime = CurrentTime;
            TotalTime += DeltaTime;
            Update(DeltaTime.count());
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    });

    std::thread inputThread([=]()
    {
        while (bRunning)
        {
            char* input = new char[256];
            scanf("%s", input);
            std::string inputStr(input);
            if(inputStr == "exit")
            {
                bRunning = false;
            }
        }
    });

    networkThread.join();
    inputThread.join();

    Clean();

    return EXIT_SUCCESS;
}

i32 SEngine::Init()
{
    SGame::Get().Start();

    if(SNetworkDriver::Get().Init() != EXIT_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    bRunning = true;

    return EXIT_SUCCESS;
}

void SEngine::Update(f32 deltaTime)
{
    SGame::Get().Update(deltaTime);
    SNetworkDriver::Get().Update(deltaTime);
}

void SEngine::Clean()
{
    SGame::Get().Clean();
    SNetworkDriver::Get().Clean();
}

void SEngine::Start()
{
}
