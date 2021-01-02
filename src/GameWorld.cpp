#include "GameWorld.h"
#include "Transform.h"
#include "AssetManagement/JsonUtils.h"
#include "FPSCameraSystem.h"
#include "DebugSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderingSystem.h"
#include "Graphics/GraphicsCore.h"

using namespace Engine::ECS;

GameWorld::GameWorld(): 
    mLevelLoader(entities)
{
}

GameWorld::~GameWorld()
= default;

void GameWorld::Init()
{
    auto fpsCameraSystem = systems.Add<FPSCameraSystem>();
    auto renderingSystem = systems.Add<RenderingSystem>();
    systems.Add<DebugSystem>();
    systems.Configure();
}

void GameWorld::LoadScene(const std::string& path)
{
    mLevelLoader.Load(path);
}

void GameWorld::Update(double deltaTime)
{
    systems.update<FPSCameraSystem>(deltaTime);
    systems.update<DebugSystem>(deltaTime);
    systems.update<RenderingSystem>(deltaTime);
}
