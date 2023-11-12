#include <engine.h>
#include <Core/Scene.h>
#include "Scenes/MainScene.h"

int main()
{
    Scene* mainScene = new MainScene();
    return Engine::Get().Run(mainScene);
}
