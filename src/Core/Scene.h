#ifndef X_SCENE_H
#define X_SCENE_H

#include "../base/defines.h"
#include <entt.hpp>
#include <SDL2/SDL_events.h>
#include "../Components/QueuedComponent.h"

namespace x
{
    class Scene
    {
    protected:
        entt::registry Registry;
        u32 MeshCount = 0;
        std::vector<entt::entity> Entities;

        u8 bShowUI : 1;

        friend class Game;
        virtual void Start() = 0;
        virtual void HandleInput(const SDL_Event& event) = 0;
        virtual void Update(f32 deltaTime);
        virtual void Clean() = 0;
        virtual void Save() = 0;
        virtual void Load() = 0;
        virtual void DrawUI() = 0;

    public:
        Scene();

        inline entt::registry& GetRegistry() { return Registry; }

        entt::entity CreateEntity()
        {
            entt::entity e = Registry.create();
            Entities.push_back(e);
            return e;
        }

        void RemoveEntity(entt::entity entity)
        {
            Registry.destroy(entity);
        }

        template<typename T>
        void AddComponent(entt::entity entity, T component)
        {
            Registry.emplace<T>(entity, component);
        }

        template<typename T>
        void RemoveComponent(entt::entity entity)
        {
            Registry.remove<T>(entity);
        }

        template<typename T>
        T& GetComponent(entt::entity entity)
        {
            return Registry.get<T>(entity);
        }

        entt::entity QueueEntityForCreation(u32 compFlags);

        void AddQueuedComponents(entt::entity entity, const std::bitset<128>& bitset);

        void ResolveQueuedEntities();

    };
}

#endif //X_SCENE_H
