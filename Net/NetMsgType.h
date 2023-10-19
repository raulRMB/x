//
// Created by Raul Romero on 2023-10-19.
//

#ifndef X_NET_EVENT_H
#define X_NET_EVENT_H

#include "base/defines.h"
#include "NetMessage.h"

struct NetSpawnMessage : public NetMessage
{
    u32 EntityId;
    NetSpawnMessage(u32 entity = 0) : NetMessage(ENetMsg::SpawnEntity), EntityId(entity) {}
};

#endif //X_NET_EVENT_H
