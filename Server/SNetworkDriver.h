//
// Created by Raul Romero on 2023-10-18.
//

#ifndef X_NETWORK_DRIVER_H
#define X_NETWORK_DRIVER_H

#include <base/defines.h>
#include <enet/enet.h>
#include <vendor/entt.hpp>

class SNetworkDriver
{
    ENetHost* pServer;
    std::vector<ENetPeer*> pPeers;
    ENetAddress Address;
    ENetEvent Event;

public:
    SNetworkDriver();
    ~SNetworkDriver() = default;

    static SNetworkDriver& Get()
    {
        static SNetworkDriver instance;
        return instance;
    }

    i32 Run();

    i32 Init();
    void Update(f32 deltaTime);
    void Clean();

    void UpdateEntities(f32 deltaTime);
    void UpdateEntity(entt::entity entity);
};

#endif //X_NETWORK_DRIVER_H
