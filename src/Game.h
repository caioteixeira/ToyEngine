#pragma once
#include "WindowsHeaders.h"
#include "GameWorld.h"
#include "InputManager.h"

namespace Engine
{
    class Game
    {
    public:
        Game();
        ~Game();
        bool Init();
        void RunLoop();
        void QuitGame() { mShouldQuit = true; }

    private:
        void StartGame();

        ECS::GameWorld mWorld;
        InputManager mInput;

        bool mShouldQuit;
    };
}
