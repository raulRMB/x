//
// Created by Raul Romero on 2023-10-19.
//

#include "SNetEntityManager.h"
#include "Net/NetMessage.h"
#include "Net/NetMsgType.h"


void SNetEntityManager::SpawnEntity(entt::entity entity, ENetPeer *peer)
{
    NetSpawnMessage message = NetSpawnMessage((u32)entity);

    ENetPacket *packet = enet_packet_create(&message, sizeof(message), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void SNetEntityManager::AddComponent(entt::entity entity, ENetCompId compId, ENetPeer *peer)
{
//    NetMessage message{};
//
//    ENetPacket* packet = enet_packet_create(&message, message.Size(), ENET_PACKET_FLAG_RELIABLE);
//    enet_peer_send(peer, 0, packet);
}
