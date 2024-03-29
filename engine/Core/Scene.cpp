#include "Scene.h"
#include "../Components/MeshComponent.h"
#include "imgui.h"
#include "../Engine.h"

Scene::Scene() : bShowUI(false){}

void Scene::Update(f32 deltaTime)
{
    ResolveQueuedEntities();
}

void Scene::ResolveQueuedEntities()
{
    auto view = Registry.view<CQueued>();
    for(auto e : view)
    {
        AddQueuedComponents(e, Registry.get<CQueued>(e).Components);
    }
}

void Scene::AddQueuedComponents(entt::entity entity, const std::bitset<128> &bitset)
{
    for(u32 i = (u32)EComponent::None; i < (u32)EComponent::ComponentCount; i++)
    {
        if(bitset.test(i))
        {
            switch(EComponent(i))
            {
                case EComponent::LineMesh:
                {
                    AddComponent(entity, CLineMesh{0});
                    break;
                }
                case EComponent::TriangleMesh:
                {
                    AddComponent(entity, CTriangleMesh{0});
                    break;
                }
                default:
                    break;
            }
        }
    }
    Registry.remove<CQueued>(entity);
}

entt::entity Scene::QueueEntityForCreation(u32 compFlags)
{
    entt::entity e = CreateEntity();
    CQueued queued{};
    for(u32 i = (u32)EComponent::None; i < (u32)EComponent::ComponentCount; i++)
    {
        if(compFlags & (u32)EComponent(i))
        {
            queued.Components.set((u32)EComponent(i));
        }
    }
    AddComponent(e, queued);
    Entities.push_back(e);
    return e;
}
