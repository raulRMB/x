//
// Created by Raul Romero on 2023-10-18.
//

#include "SNetworkDriver.h"
#include <iostream>
#include <Server/SEngine.h>
#include "Components/PhysicsComponent.h"
#include "Components/TransformComponent.h"
#include "Components/NetworkComponent.h"
#include "SNetEntityManager.h"
#include "SGame.h"

SNetworkDriver::SNetworkDriver() : pServer(nullptr), pPeers(std::vector<ENetPeer*>()), Address(), Event()
{
}

i32 SNetworkDriver::Run()
{
    return EXIT_SUCCESS;
}

i32 SNetworkDriver::Init()
{
    if(enet_initialize() != 0)
    {
        printf("Error initializing ENet");
        return EXIT_FAILURE;
    }
    std::atexit(enet_deinitialize);

    Address.host = ENET_HOST_ANY;
    Address.port = 7777;

    pServer = enet_host_create(&Address, 32, 1, 0, 0);

    if(pServer == nullptr)
    {
        printf("Error creating server");
        return EXIT_FAILURE;
    }

    printf("Server started on port %u\n", Address.port);
    return EXIT_SUCCESS;
}

bool started = false;
void SNetworkDriver::Update(f32 deltaTime)
{
    while(enet_host_service(pServer, &Event, 0) > 0)
    {
        switch (Event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %u:%u\n", Event.peer->address.host,  Event.peer->address.port);
                started = true;
                pPeers.push_back(Event.peer);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                printf("A packet of length %zu containing %s was received from %u:%u at channel Id: %u\n",
                       Event.packet->dataLength,
                       Event.packet->data,
                       Event.peer->address.host,
                       Event.peer->address.port,
                       Event.channelID);
                enet_packet_destroy(Event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%u:%u disconnected.\n", Event.peer->address.host, Event.peer->address.port);
                Event.peer->data = nullptr;
                if(pServer->peerCount == 0)
                {
                    printf("No more clients connected.\n");
                    enet_host_destroy(pServer);
                    return;
                }
                break;
            default:
                break;
        }
    }
    if(started)
    {
        UpdateEntities(deltaTime);
    }
}

void SNetworkDriver::Clean()
{

}

void SNetworkDriver::UpdateEntities(f32 deltaTime)
{
    auto view = SGame::Get().GetRegistry().view<CTransform3d, CPhysics3d>();
    for(auto entity : view)
    {
        auto& transform = view.get<CTransform3d>(entity);
        auto& physics = view.get<CPhysics3d>(entity);

        transform.WorldPosition += physics.Velocity * deltaTime;
        UpdateEntity(entity);
    }
}

void SNetworkDriver::UpdateEntity(entt::entity entity)
{
    auto& transform = SGame::Get().GetRegistry().get<CTransform3d>(entity);
    auto& network = SGame::Get().GetRegistry().get<CNetwork>(entity);

    struct {
        CNetwork network;
        CTransform3d transform;
    } transformNetwork{network, transform};

    ENetPacket* packet = enet_packet_create(&transformNetwork, sizeof(transformNetwork), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(pServer, 0, packet);
}
