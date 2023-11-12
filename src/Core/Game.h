#ifndef X_GAME_H
#define X_GAME_H

#include "../Core/Scene.h"
#include "../Enums/SceneId.h"

namespace x
{
    class Game
    {
        class Scene* CurrentScene;
        SceneId CurrentSceneState;
    public:
        Game() = default;
        Game(const Game &) = delete;

        Game &operator=(const Game &) = delete;

        static Game &GetInstance();
        void Init();
        void Start();
        void HandleInput(const SDL_Event& event);
        void Update(f32 deltaTime);
        void Clean();

        void SetScene(SceneId sceneId);
        [[nodiscard]] inline Scene* GetScene() const { return CurrentScene; }

        void Save();
        void Load();

        void DrawUI();
    };
}

#endif //X_GAME_H
