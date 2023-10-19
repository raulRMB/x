//
// Created by Raul Romero on 2023-10-17.
//

#include "NetworkDriver.h"
#include "Server/SNetworkDriver.h"
#include "Components/NetworkComponent.h"
#include "Components/TransformComponent.h"
#include "Net/NetMessage.h"
#include "Net/NetMsgType.h"

#include <enet/enet.h>
#include <vendor/entt.hpp>
#include <cstdio>

#include "core/Game.h"

i32 NetworkDriver::Init()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    std::atexit(enet_deinitialize);

    if (pClient = enet_host_create(nullptr, 1, 1, 0, 0); !pClient)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        return EXIT_FAILURE;
    }

    enet_address_set_host(&Address, "localhost");
    Address.port = 7777;

    if (pPeer = enet_host_connect(pClient, &Address, 1, 0); !pPeer)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        return EXIT_FAILURE;
    }

    if(enet_host_service(pClient, &Event, 5000) > 0 &&
        Event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts("Connection to localhost:7777 succeeded.");
    }
    else
    {
        enet_peer_reset(pPeer);
        puts("Connection to localhost:7777 failed.");
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

void NetworkDriver::Loop()
{
    if(enet_host_service(pClient, &Event, 0) > 0)
    {
        switch(Event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                printf("A packet of length %u containing %s was received from %u:%u on channel %u.\n",
                       (u32) Event.packet->dataLength,
                       Event.packet->data,
                       Event.peer->address.host,
                       Event.peer->address.port,
                       Event.channelID);

                ENetMsg type = *(ENetMsg *) Event.packet->data;
                switch (type)
                {
                    case ENetMsg::SpawnEntity:
                    {
                        NetSpawnMessage message = *(NetSpawnMessage *) Event.packet->data;
                        printf("Entity spawned Id: %d\n", message.EntityId);
                        entt::entity e = x::Game::GetInstance().GetScene()->CreateEntity();
                        auto s = x::Game::GetInstance().GetScene();
                        s->AddComponent(e, CNetwork{message.EntityId});
                        break;
                    }
                    case ENetMsg::KillEntity:
                    {
                        printf("Kill Entity\n");
                        break;
                    }
                    case ENetMsg::UpdateEntity:
                    {
                        printf("UpdateEntity\n");
                        break;
                    }
                    case ENetMsg::AddComponent:
                    {
                        printf("AddComponent\n");
                        break;
                    }
                    default:
                        break;
                }
                enet_packet_destroy(Event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", (char *) Event.peer->data);
                Event.peer->data = nullptr;
                break;
            default:
                break;
        }
    }
}
