//
// Created by Raul Romero on 2023-10-17.
//

#ifndef X_NETWORK_DRIVER_H
#define X_NETWORK_DRIVER_H

#include <base/defines.h>
#include <enet/enet.h>

namespace x { class Engine; }

class NetworkDriver
{
    ENetHost* pClient;
    ENetPeer* pPeer;
    ENetAddress Address;
    ENetEvent Event;

public:
    v3 PlayerPosition;

    NetworkDriver() = default;
    ~NetworkDriver() = default;

    static NetworkDriver& Get()
    {
        static NetworkDriver instance;
        return instance;
    }

    friend class x::Engine;
    i32 Init();
    void Loop();
};


#endif //X_NETWORK_DRIVER_H
