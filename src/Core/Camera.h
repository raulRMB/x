#ifndef X_CAMERA_H
#define X_CAMERA_H

#include "../base/defines.h"
#include <entt.hpp>
#include <SDL2/SDL_Events.h>
#include "../Components/TransformComponent.h"
#include "../Components/AxesComponent.h"

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
    u8 bDebugCamera : 1;
public:
    static CameraSystem& Get()
    {
        static CameraSystem instance;
        return instance;
    }

    CameraSystem();
    entt::entity CreateCamera(const CCamera& camera, const CTransform3d& transform, const CAxes& axes);
    void SwitchCamera(entt::entity camera);
    void NextCamera();

    void MoveCamera(const SDL_Event& event, f32 deltaTime);

    inline entt::entity GetMainCamera() { return MainCamera; }
    inline CCamera& GetCameraComponent(entt::entity camera) { return pRegistry->get<CCamera>(camera); }
    inline CTransform3d& GetCameraTransform(entt::entity camera) { return pRegistry->get<CTransform3d>(camera); }
    inline v3& GetCameraWorldPosition(entt::entity camera) { return pRegistry->get<CTransform3d>(camera).WorldPosition; }
    inline v3& GetCameraWorldRotation(entt::entity camera) { return pRegistry->get<CTransform3d>(camera).WorldRotation; }
    inline v3& GetCameraLocalPosition(entt::entity camera) { return pRegistry->get<CTransform3d>(camera).LocalPosition; }
    inline v3& GetCameraLocalRotation(entt::entity camera) { return pRegistry->get<CTransform3d>(camera).LocalRotation; }
    inline m4& GetCameraView(entt::entity camera) { return pRegistry->get<CCamera>(camera).View; }
    inline m4& GetCameraProjection(entt::entity camera) { return pRegistry->get<CCamera>(camera).Projection; }
    inline v3& GetCameraForward(entt::entity camera) { return pRegistry->get<CCamera>(camera).Forward; }
    inline v3& GetCameraRight(entt::entity camera) { return pRegistry->get<CCamera>(camera).Right; }
    inline v3& GetCameraUp(entt::entity camera) { return pRegistry->get<CCamera>(camera).Up; }
    inline f32& GetCameraFOV(entt::entity camera) { return pRegistry->get<CCamera>(camera).FOV; }
    inline f32& GetCameraNear(entt::entity camera) { return pRegistry->get<CCamera>(camera).Near; }
    inline f32& GetCameraFar(entt::entity camera) { return pRegistry->get<CCamera>(camera).Far; }
    inline CAxes& GetCameraAxes(entt::entity camera) { return pRegistry->get<CAxes>(camera); }

    inline void SetCameraPosition(entt::entity camera, const v3& position) { GetCameraWorldPosition(camera) = position; }

    inline void AddCameraPosition(entt::entity camera, const v3& position) { GetCameraWorldPosition(camera) += position; }

    inline CCamera& GetMainCameraComponent() { return GetCameraComponent(MainCamera); }
    inline CTransform3d& GetMainCameraTransform() { return GetCameraTransform(MainCamera); }
    inline v3& GetMainCameraPosition() { return GetCameraWorldPosition(MainCamera); }
    inline v3& GetMainCameraRotation() { return GetCameraWorldRotation(MainCamera); }
    inline v3& GetMainCameraLocalPosition() { return GetCameraLocalPosition(MainCamera); }
    inline v3& GetMainCameraLocalRotation() { return GetCameraLocalRotation(MainCamera); }
    inline m4& GetMainCameraView() { return GetCameraView(MainCamera); }
    inline m4& GetMainCameraProjection() { return GetCameraProjection(MainCamera); }
    inline v3& GetMainCameraForward() { return GetCameraForward(MainCamera); }
    inline v3& GetMainCameraRight() { return GetCameraRight(MainCamera); }
    inline v3& GetMainCameraUp() { return GetCameraUp(MainCamera); }
    inline f32& GetMainCameraFOV() { return GetCameraFOV(MainCamera); }
    inline f32& GetMainCameraNear() { return GetCameraNear(MainCamera); }
    inline f32& GetMainCameraFar() { return GetCameraFar(MainCamera); }

    inline CAxes& GetMainCameraAxes() { return GetCameraAxes(MainCamera); }

    inline void SetCameraFOV(entt::entity camera, f32 fov) { pRegistry->get<CCamera>(camera).FOV = fov; }
    inline void SetCameraNear(entt::entity camera, f32 near) { pRegistry->get<CCamera>(camera).Near = near; }
    inline void SetCameraFar(entt::entity camera, f32 far) { pRegistry->get<CCamera>(camera).Far = far; }

    inline void SetMainCameraFOV(f32 fov) { SetCameraFOV(MainCamera, fov); }
    inline void SetMainCameraNear(f32 near) { SetCameraNear(MainCamera, near); }
    inline void SetMainCameraFar(f32 far) { SetCameraFar(MainCamera, far); }

    inline void SetMainCameraPosition(const v3& position) { SetCameraPosition(MainCamera, position); }

    inline void AddMainCameraPosition(const v3& position) { AddCameraPosition(MainCamera, position); }

    void UpdateCamera(entt::entity camera);
    void UpdateMainCamera();

    m4 GetVPI();
    m4 GetVP();

    static bool DebugCameraActive();
};

#endif //X_CAMERA_H
