#include "../engine/engine.h"
#include "Scenes/MainScene.h"

int main()
{
    Scene* mainScene = new MainScene();
    return x::Engine::Get().Run(mainScene);
}
