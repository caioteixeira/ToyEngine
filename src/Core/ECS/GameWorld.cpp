#include "GameWorld.h"
#include "../../RenderingSystem.h"

using namespace Engine::ECS;

GameWorld::GameWorld()
{
	systems.add<RenderingSystem>();
	systems.configure();
}


GameWorld::~GameWorld()
{
}

void GameWorld::Update(float deltaTime)
{
	systems.update<RenderingSystem>(deltaTime);
}


