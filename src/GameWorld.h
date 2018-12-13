#pragma once
#include "ECS/entityx.h"
#include "Renderer/Renderer.h"
#include "AssetManagement/LevelLoader.h"

namespace Engine::ECS
{
    class GameWorld : public entityx::EntityX
    {
    public:
        GameWorld();
        ~GameWorld();

        void Init(std::shared_ptr<Renderer> renderer);
        void LoadMesh(entityx::Entity rootEntity, rapidjson::Value &);
        void LoadScene(const std::string& path);
        void Update(double deltaTime);
    private:
        std::shared_ptr<Renderer> mRenderer;
        LevelLoader mLevelLoader;
    };
}
