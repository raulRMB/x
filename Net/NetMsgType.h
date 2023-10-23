//
// Created by Raul Romero on 2023-10-19.
//

#ifndef X_NET_EVENT_H
#define X_NET_EVENT_H

#include "base/defines.h"
#include "NetMessage.h"
#include "Components/TransformComponent.h"

struct NetSpawnMessage : public NetMessage
{
    u32 EntityId;
    CTransform3d Transform;
    NetSpawnMessage(u32 entity = 0, CTransform3d transform = CTransform3d()) : NetMessage(ENetMsg::SpawnEntity),
        EntityId(entity), Transform(transform) {}
};

struct NetUpdateMessage : public NetMessage
{
    u32 EntityId;
    CTransform3d Transform;
    NetUpdateMessage(u32 entity = 0, CTransform3d transform = CTransform3d()) : NetMessage(ENetMsg::UpdateEntity),
        EntityId(entity), Transform(transform) {}
};

#endif //X_NET_EVENT_H
