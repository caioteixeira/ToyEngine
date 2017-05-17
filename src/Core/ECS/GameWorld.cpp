#include "GameWorld.h"
#include "../../RenderingSystem.h"

using namespace Engine::ECS;

GameWorld::GameWorld()
{
	systems.Add<RenderingSystem>();
	systems.Configure();
}


GameWorld::~GameWorld()
{
}

void GameWorld::Update(float deltaTime)
{
	systems.update<RenderingSystem>(deltaTime);
}


