#include "Camera.h"
#include "Game.h"
#include "../Components/TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

CameraSystem::CameraSystem() : MainCamera(entt::null), bDebugCamera(false)
{
    pRegistry = &x::Game::GetInstance().GetScene()->GetRegistry();
    CTransform3d transform{};
    transform.WorldPosition = v3(250.0f, -250.0f, -50.0f);
    transform.WorldRotation = v3(0.0f, 0.0f, 0.0f);
    transform.WorldScale = v3(1.0f, 1.0f, 1.0f);

    CCamera camera{};
    camera.Forward = v3(0.0f, 0.0f, -1.0f);
    camera.Right = v3(1.0f, 0.0f, 0.0f);
    camera.Up = v3(0.0f, 1.0f, 0.0f);
    camera.View = glm::lookAt(transform.WorldPosition, camera.Forward, camera.Up);
    camera.FOV = 30.0f;
    camera.Near = 0.1f;
    camera.Far = 10000.0f;
    camera.Projection = glm::perspective(glm::radians(camera.FOV), WINDOW_WIDTH_F / WINDOW_HEIGHT_F, camera.Near, camera.Far);
    camera.Projection[1][1] *= -1;

    CAxes axes{};
    axes.Yaw = -90.f;
    axes.Pitch = 55.f;
    axes.Roll = 0.f;

    SwitchCamera(CreateCamera(camera, transform, axes));
}

entt::entity CameraSystem::CreateCamera(const CCamera& camera, const CTransform3d& transform, const CAxes& axes)
{
    auto e = pRegistry->create();
    pRegistry->emplace<CCamera>(e, camera);
    pRegistry->emplace<CTransform3d>(e, transform);
    pRegistry->emplace<CAxes>(e, axes);
    Cameras.push_back(e);
    return e;
}

void CameraSystem::SwitchCamera(entt::entity camera)
{
    MainCamera = camera;
}

glm::vec3 RotateVectorWithQuaternion(const glm::vec3& vectorToRotate, const glm::quat& rotationQuaternion) {
    // Convert the vector to a quaternion to perform the rotation
    glm::vec4 vec4ToRotate(vectorToRotate, 0.0f); // 0.0f for the homogeneous coordinate
    glm::vec4 rotatedVectorQuaternion = rotationQuaternion * vec4ToRotate * glm::conjugate(rotationQuaternion);

    // Extract the rotated vector from the quaternion
    glm::vec3 rotatedVector(rotatedVectorQuaternion.x, rotatedVectorQuaternion.y, rotatedVectorQuaternion.z);

    return rotatedVector;
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
        case SDLK_RIGHTBRACKET:
            bDebugCamera = !bDebugCamera;
            break;
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
            CreateCamera(GetMainCameraComponent(), GetMainCameraTransform(), GetMainCameraAxes());
            break;
        case SDLK_m:
            NextCamera();
            break;
        default:
            break;
        }
    }
    if(!bDebugCamera)
    {
        return;
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

    if(event.type == SDL_MOUSEMOTION)
    {
        if(event.motion.state & SDL_BUTTON_RMASK)
        {
            GetMainCameraAxes().Yaw += (f32)event.motion.xrel * 0.1f;
            GetMainCameraAxes().Pitch += (f32)event.motion.yrel * 0.1f;
        }
    }

    v3& pos = GetMainCameraPosition();
    f32 speed = 100.f;

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
}

void CameraSystem::UpdateCamera(entt::entity camera)
{
    auto& transform = pRegistry->get<CTransform3d>(camera);
    auto& cameraComponent = pRegistry->get<CCamera>(camera);
    auto& axes = pRegistry->get<CAxes>(camera);

    transform.WorldRotation = {glm::radians(axes.Yaw), glm::radians(axes.Pitch), glm::radians(axes.Roll)};

    cameraComponent.Forward.x = cosf(glm::radians(axes.Yaw)) * cosf(glm::radians(axes.Pitch));
    cameraComponent.Forward.y = sinf(glm::radians(axes.Pitch));
    cameraComponent.Forward.z = sinf(glm::radians(axes.Yaw)) * cosf(glm::radians(axes.Pitch));
    cameraComponent.Forward = glm::normalize(cameraComponent.Forward);

    cameraComponent.Right = glm::normalize(glm::cross(cameraComponent.Forward, v3(0.0f, 1.0f, 0.0f)));
    cameraComponent.Up = glm::normalize(glm::cross(cameraComponent.Right, cameraComponent.Forward));

    cameraComponent.View = glm::lookAt(transform.WorldPosition, transform.WorldPosition + cameraComponent.Forward, cameraComponent.Up);
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
