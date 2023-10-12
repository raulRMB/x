//
// Created by Raul Romero on 2023-10-12.
//

#ifndef X_CAMERA_H
#define X_CAMERA_H

#include <base/defines.h>
#include "../vendor/entt.hpp"
#include <SDL2/SDL_events.h>
#include "Components/TransformComponent.h"

struct CCamera
{
    v3 Forward;
    v3 Right;
    v3 Up;

    m4 View;
    m4 Projection;

    f32 FOV;
    f32 Near;
    f32 Far;
};

class CameraSystem
{
    entt::entity MainCamera;
    entt::registry* pRegistry;
    std::vector<entt::entity> Cameras;
    u32 Mask = 0u;
public:
    static CameraSystem& Get()
    {
        static CameraSystem instance;
        return instance;
    }

    CameraSystem();
    entt::entity CreateCamera(const CCamera& camera, const CTransform3d& transform);
    void SwitchCamera(entt::entity camera);
    void NextCamera();

    void MoveCamera(const SDL_Event& event, f32 deltaTime);

    inline entt::entity GetMainCamera() { return MainCamera; }
    inline CCamera& GetCameraComponent(entt::entity camera) { return pRegistry->get<CCamera>(camera); }
    inline CTransform3d& GetCameraTransform(entt::entity camera) { return pRegistry->get<CTransform3d>(camera); }
    inline v3& GetCameraPosition(entt::entity camera) { return pRegistry->get<CTransform3d>(camera).Position; }
    inline v3& GetCameraRotation(entt::entity camera) { return pRegistry->get<CTransform3d>(camera).Rotation; }
    inline m4& GetCameraView(entt::entity camera) { return pRegistry->get<CCamera>(camera).View; }
    inline m4& GetCameraProjection(entt::entity camera) { return pRegistry->get<CCamera>(camera).Projection; }
    inline v3& GetCameraForward(entt::entity camera) { return pRegistry->get<CCamera>(camera).Forward; }
    inline v3& GetCameraRight(entt::entity camera) { return pRegistry->get<CCamera>(camera).Right; }
    inline v3& GetCameraUp(entt::entity camera) { return pRegistry->get<CCamera>(camera).Up; }
    inline f32& GetCameraFOV(entt::entity camera) { return pRegistry->get<CCamera>(camera).FOV; }
    inline f32& GetCameraNear(entt::entity camera) { return pRegistry->get<CCamera>(camera).Near; }
    inline f32& GetCameraFar(entt::entity camera) { return pRegistry->get<CCamera>(camera).Far; }

    inline CCamera& GetMainCameraComponent() { return pRegistry->get<CCamera>(MainCamera); }
    inline CTransform3d& GetMainCameraTransform() { return pRegistry->get<CTransform3d>(MainCamera); }
    inline v3& GetMainCameraPosition() { return pRegistry->get<CTransform3d>(MainCamera).Position; }
    inline v3& GetMainCameraRotation() { return pRegistry->get<CTransform3d>(MainCamera).Rotation; }
    inline m4& GetMainCameraView() { return pRegistry->get<CCamera>(MainCamera).View; }
    inline m4& GetMainCameraProjection() { return pRegistry->get<CCamera>(MainCamera).Projection; }
    inline v3& GetMainCameraForward() { return pRegistry->get<CCamera>(MainCamera).Forward; }
    inline v3& GetMainCameraRight() { return pRegistry->get<CCamera>(MainCamera).Right; }
    inline v3& GetMainCameraUp() { return pRegistry->get<CCamera>(MainCamera).Up; }
    inline f32& GetMainCameraFOV() { return pRegistry->get<CCamera>(MainCamera).FOV; }
    inline f32& GetMainCameraNear() { return pRegistry->get<CCamera>(MainCamera).Near; }
    inline f32& GetMainCameraFar() { return pRegistry->get<CCamera>(MainCamera).Far; }

    inline void SetCameraFOV(entt::entity camera, f32 fov) { pRegistry->get<CCamera>(camera).FOV = fov; }
    inline void SetCameraNear(entt::entity camera, f32 near) { pRegistry->get<CCamera>(camera).Near = near; }
    inline void SetCameraFar(entt::entity camera, f32 far) { pRegistry->get<CCamera>(camera).Far = far; }

    inline void SetMainCameraFOV(f32 fov) { SetCameraFOV(MainCamera, fov); }
    inline void SetMainCameraNear(f32 near) { SetCameraNear(MainCamera, near); }
    inline void SetMainCameraFar(f32 far) { SetCameraFar(MainCamera, far); }

    void UpdateCamera(entt::entity camera);
    void UpdateMainCamera();

    m4 GetVPI();
    m4 GetVP();
};

#endif //X_CAMERA_H
