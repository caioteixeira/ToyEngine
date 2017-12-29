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

	const Vector3 cameraPos(30.0 , 30.0, 100.0);
	cameraEntity.Assign<Transform>()->position = cameraPos;
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

	for (Mesh& mesh : meshes)
	{
		entityx::Entity meshEntity = entities.Create();
		meshEntity.Assign<Mesh>(mesh);
		meshEntity.Assign<Transform>();
	}
}

void GameWorld::Update(double deltaTime)
{
	systems.update<RenderingSystem>(deltaTime);
}


