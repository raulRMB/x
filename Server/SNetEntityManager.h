//
// Created by Raul Romero on 2023-10-19.
//

#ifndef X_SNETENTITY_MANAGER_H
#define X_SNETENTITY_MANAGER_H

#include "vendor/entt.hpp"
#include "enet/enet.h"
#include "Net/NetMessage.h"
#include "Net/NetCompId.h"

class SNetEntityManager
{
public:
    static void SpawnEntity(entt::entity entity, ENetPeer* peer);
    static void AddComponent(entt::entity entity, ENetCompId compId, ENetPeer* peer);
};


#endif //X_SNETENTITY_MANAGER_H
