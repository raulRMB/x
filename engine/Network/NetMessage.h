#ifndef X_NET_MESSAGE_H
#define X_NET_MESSAGE_H

#include "../Core/defines.h"

enum class ENetMsg : u32
{
    None = 0,
    SpawnEntity,
    KillEntity,
    UpdateEntity,
    AddComponent,
};

struct NetMessage
{
    ENetMsg Type = ENetMsg::None;

    NetMessage(ENetMsg type = ENetMsg::None) : Type(type) {}
};

#endif //X_NET_MESSAGE_H