#include "NetworkDriver.h"
#include "../Components/NetworkComponent.h"
#include "../Components/TransformComponent.h"
#include "NetMessage.h"
#include "NetMsgType.h"

#include <../../vendor/entt/entt.hpp>
#include <cstdio>

#include "../Core/Game.h"
#include "../Components/MeshComponent.h"

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
                ENetMsg type = *(ENetMsg *) Event.packet->data;
                switch (type)
                {
                    case ENetMsg::SpawnEntity:
                    {
                        NetSpawnMessage message = *(NetSpawnMessage*) Event.packet->data;
                        printf("Entity spawned Id: %d\n", message.EntityId);
                        auto s = x::Game::GetInstance().GetScene();
                        entt::entity e = s->CreateEntity();
                        s->AddComponent(e, CNetwork{message.EntityId});
                        s->AddComponent(e, message.Transform);
                        s->AddComponent(e, CLineMesh{0});
                        break;
                    }
                    case ENetMsg::KillEntity:
                    {
                        printf("Kill Entity\n");
                        break;
                    }
                    case ENetMsg::UpdateEntity:
                    {
                        NetUpdateMessage message = *(NetUpdateMessage*) Event.packet->data;
                        auto s = x::Game::GetInstance().GetScene();
                        auto view = s->GetRegistry().view<CNetwork, CTransform3d>();
                        for(auto e : view)
                        {
                            if(view.get<CNetwork>(e).Id == message.EntityId)
                            {
                                view.get<CTransform3d>(e) = message.Transform;
                                break;
                            }
                        }
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
