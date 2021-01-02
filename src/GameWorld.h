#pragma once
#include "ECS/entityx.h"
#include "AssetManagement/LevelLoader.h"
#include "Graphics/Renderer.h"

namespace Engine::ECS
{
    class GameWorld : public entityx::EntityX
    {
    public:
        GameWorld();
        ~GameWorld();

        void Init();
        void LoadScene(const std::string& path);
        void Update(double deltaTime);
    private:
        LevelLoader mLevelLoader;
    };
}
