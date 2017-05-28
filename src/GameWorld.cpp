#include "GameWorld.h"
#include "RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"

using namespace Engine::ECS;

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init(std::shared_ptr<Renderer> renderer)
{
	entityx::Entity cameraEntity = entities.Create();

	Vector3 cameraPos(0, -10, 0);
	cameraEntity.Assign<Transform>(cameraPos);
	cameraEntity.Assign<Camera>();

	auto renderingSystem = systems.Add<RenderingSystem>();
	systems.Configure();
	renderingSystem->SetRenderer(renderer);
	mRenderer = renderer;
}

void GameWorld::LoadObjLevel(const std::string& path)
{
	auto resourceManager = mRenderer->GetResourceManager();

	std::vector<Mesh> meshes;
	resourceManager->LoadObjFile(path, meshes);

	for (Mesh mesh : meshes)
	{
		entityx::Entity meshEntity = entities.Create();
		meshEntity.Assign<Mesh>(mesh);
		meshEntity.Assign<Transform>(Vector3::Zero);
	}
}

void GameWorld::Update(float deltaTime)
{
	systems.update<RenderingSystem>(deltaTime);
}


