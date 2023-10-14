//
// Created by Raul Romero on 2023-10-12.
//

#include "Camera.h"
#include "Game.h"
#include "Components/TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>

CameraSystem::CameraSystem() : MainCamera(entt::null)
{
    pRegistry = &x::Game::GetInstance().GetScene()->GetRegistry();
    CTransform3d transform{};
    transform.Position = v3(0.0f, 0.0f, 3.0f);
    transform.Rotation = v3(0.0f, glm::radians(-90.f), 0.0f);
    transform.Scale = v3(1.0f, 1.0f, 1.0f);

    CCamera camera{};
    camera.Forward = v3(0.0f, 0.0f, -1.0f);
    camera.Right = v3(1.0f, 0.0f, 0.0f);
    camera.Up = v3(0.0f, 1.0f, 0.0f);
    camera.View = glm::lookAt(transform.Position, camera.Forward, camera.Up);
    camera.FOV = 30.0f;
    camera.Near = 0.1f;
    camera.Far = 100.0f;
    camera.Projection = glm::perspective(glm::radians(camera.FOV), WINDOW_WIDTH_F / WINDOW_HEIGHT_F, camera.Near, camera.Far);

    SwitchCamera(CreateCamera(camera, transform));
}

entt::entity CameraSystem::CreateCamera(const CCamera& camera, const CTransform3d& transform)
{
    auto e = pRegistry->create();
    pRegistry->emplace<CCamera>(e, camera);
    pRegistry->emplace<CTransform3d>(e, transform);
    Cameras.push_back(e);
    return e;
}

void CameraSystem::SwitchCamera(entt::entity camera)
{
    MainCamera = camera;
}

void CameraSystem::NextCamera()
{
    if(Cameras.size() > 1)
    {
        auto it = std::find(Cameras.begin(), Cameras.end(), MainCamera);
        if(it != Cameras.end())
        {
            ++it;
            if(it == Cameras.end())
            {
                it = Cameras.begin();
            }
            SwitchCamera(*it);
        }
        else
        {
            SwitchCamera(Cameras[0]);
        }
    }
}

void CameraSystem::MoveCamera(const SDL_Event& event, f32 DeltaTime)
{
    if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
        case SDLK_w:
            Mask |= 1u << 0;
            break;
        case SDLK_s:
            Mask |= 1u << 1;
            break;
        case SDLK_a:
            Mask |= 1u << 2;
            break;
        case SDLK_d:
            Mask |= 1u << 3;
            break;
        case SDLK_LSHIFT:
            Mask |= 1u << 4;
            break;
        case SDLK_SPACE:
            Mask |= 1u << 5;
            break;
        case SDLK_n:
            CreateCamera(GetMainCameraComponent(), GetMainCameraTransform());
            break;
        case SDLK_m:
            NextCamera();
            break;
        default:
            break;
        }
    }
    if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
        case SDLK_w:
            Mask &= ~(1u << 0);
            break;
        case SDLK_s:
            Mask &= ~(1u << 1);
            break;
        case SDLK_a:
            Mask &= ~(1u << 2);
            break;
        case SDLK_d:
            Mask &= ~(1u << 3);
            break;
        case SDLK_LSHIFT:
            Mask &= ~(1u << 4);
            break;
        case SDLK_SPACE:
            Mask &= ~(1u << 5);
            break;
        default:
            break;
        }
    }
    if(event.type == SDL_MOUSEWHEEL)
    {
        if(event.wheel.y > 0)
        {
            SetMainCameraFOV(GetMainCameraFOV() - 1.f);
        }
        else if(event.wheel.y < 0)
        {
            SetMainCameraFOV(GetMainCameraFOV() + 1.f);
        }
    }
//    SDL_ShowCursor(SDL_DISABLE);
    if(event.type == SDL_MOUSEMOTION)
    {
        if(event.motion.state & SDL_BUTTON_RMASK)
        {
            GetMainCameraRotation().x += (f32)event.motion.yrel * 10.f * DeltaTime;
            GetMainCameraRotation().y += (f32)event.motion.xrel * 10.f * DeltaTime;
            std::clamp(GetMainCameraRotation().y, glm::radians(-85.f), glm::radians(85.f));
        }
    }

    v3& pos = GetMainCameraPosition();
    f32 speed = 1.f;

    if(Mask & 1u << 0)
    {
        pos += GetMainCameraForward() * speed * DeltaTime;
    }
    if(Mask & 1u << 1)
    {
        pos -= GetMainCameraForward() * speed * DeltaTime;
    }
    if(Mask & 1u << 2)
    {
        pos -= GetMainCameraRight() * speed * DeltaTime;
    }
    if(Mask & 1u << 3)
    {
        pos += GetMainCameraRight() * speed * DeltaTime;
    }
    if(Mask & 1u << 4)
    {
        pos += GetMainCameraUp() * speed * DeltaTime;
    }
    if(Mask & 1u << 5)
    {
        pos -= GetMainCameraUp() * speed * DeltaTime;
    }
    UpdateMainCamera();
}

void CameraSystem::UpdateCamera(entt::entity camera)
{
    auto& transform = pRegistry->get<CTransform3d>(camera);
    auto& cameraComponent = pRegistry->get<CCamera>(camera);

    cameraComponent.Forward.x = cosf(transform.Rotation.y) * cosf(transform.Rotation.x);
    cameraComponent.Forward.y = sinf(transform.Rotation.x);
    cameraComponent.Forward.z = sinf(transform.Rotation.y) * cosf(transform.Rotation.x);

    cameraComponent.Forward = glm::normalize(cameraComponent.Forward);
    cameraComponent.Right = glm::normalize(glm::cross(cameraComponent.Forward, cameraComponent.Up));
    cameraComponent.Up = glm::normalize(glm::cross(cameraComponent.Right, cameraComponent.Forward));

    cameraComponent.View = glm::lookAt(transform.Position, transform.Position + cameraComponent.Forward, cameraComponent.Up);
    cameraComponent.Projection = glm::perspective(glm::radians(cameraComponent.FOV), WINDOW_WIDTH_F / WINDOW_HEIGHT_F, cameraComponent.Near, cameraComponent.Far);
}

void CameraSystem::UpdateMainCamera()
{
    UpdateCamera(MainCamera);
}

m4 CameraSystem::GetVPI()
{
    return glm::inverse(GetVP());
}

m4 CameraSystem::GetVP()
{
    return GetMainCameraProjection() * GetMainCameraView();
}
