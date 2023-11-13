#ifndef X_NETWORK_DRIVER_H
#define X_NETWORK_DRIVER_H

#include "../Core/defines.h"
#include <../../vendor/enet/include/enet/enet.h>

class Engine;

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

    friend class Engine;
    i32 Init();
    void Loop();
};


#endif //X_NETWORK_DRIVER_H