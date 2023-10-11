//
// Created by Raul Romero on 2023-10-11.
//

#ifndef X_SCENE_H
#define X_SCENE_H

#include <base/defines.h>
#include <vendor/entt.hpp>

namespace x
{
    class Scene
    {
    protected:
        entt::registry Registry;
        u32 Models;
        std::vector<entt::entity> Entities;

        friend class Game;
        virtual void Start() = 0;
        virtual void Update(f32 deltaTime) = 0;
        virtual void Clean() = 0;

    public:
        inline entt::registry& GetRegistry() { return Registry; }

        entt::entity CreateEntity()
        {
            entt::entity e = Registry.create();
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
    };
}

#endif //X_SCENE_H
