#include "GameWorld.h"
#include "RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"

using namespace Engine::ECS;

GameWorld::GameWorld()
{
	entityx::Entity cameraEntity = entities.Create();

	Vector3 cameraPos(0, -10, 0);
	cameraEntity.Assign<Transform>(cameraPos);
	cameraEntity.Assign<Camera>();

	systems.Add<RenderingSystem>();
	systems.Configure();

	systems.System<RenderingSystem>();
}

GameWorld::~GameWorld()
{
}

void GameWorld::SetRenderer(std::shared_ptr<Renderer> renderer)
{
	auto renderingSystem = systems.System<RenderingSystem>();
	renderingSystem->SetRenderer(renderer);
}

void GameWorld::Update(float deltaTime)
{
	systems.update<RenderingSystem>(deltaTime);
}


