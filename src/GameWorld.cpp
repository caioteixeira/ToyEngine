#include "GameWorld.h"
#include "RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "ConstantRotation.h"
#include "RotateObjectSystem.h"

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

	Vector3 cameraPos(30.0 , 30.0, 60.0);
	cameraEntity.Assign<Transform>()->position = cameraPos;
	cameraEntity.Assign<Camera>();
	//TODO: Temporarilly look at 0,0 every frame

	auto renderingSystem = systems.Add<RenderingSystem>();
	systems.Configure();
	renderingSystem->SetRenderer(renderer);
	mRenderer = renderer;

	systems.Add<RotateObjectSystem>();
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
		meshEntity.Assign<ConstantRotation>(0.1f, 0.0f, 0.0f);
	}
}

void GameWorld::Update(double deltaTime)
{
	systems.update<RotateObjectSystem>(deltaTime);
	systems.update<RenderingSystem>(deltaTime);
}


