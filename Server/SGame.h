//
// Created by Raul Romero on 2023-10-19.
//

#ifndef X_SGAME_H
#define X_SGAME_H

#include "base/defines.h"
#include <vendor/entt.hpp>

class SGame
{
    class SMainScene* pMainScene;
public:
    SGame();
    ~SGame();

    void Start();
    void Update(f32 deltaTime);
    void Clean();

    static SGame& Get()
    {
        static SGame instance;
        return instance;
    }
public:
    [[nodiscard]] SMainScene* GetMainScene() const { return pMainScene; }
    [[nodiscard]] entt::registry& GetRegistry();
};


#endif //X_SGAME_H
