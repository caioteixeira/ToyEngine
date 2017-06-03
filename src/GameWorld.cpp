#include "GameWorld.h"
#include "RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "ConstantRotation.h"
#include "Renderer/D3DCommon/RotateObjectSystem.h"

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

	Vector3 cameraPos(2.0 , 2.0, 2.0);
	cameraEntity.Assign<Transform>()->position = cameraPos;
	auto camera = cameraEntity.Assign<Camera>();
	//TODO: Temporarilly look at 0,0 every frame
	camera.get()->viewMatrix = Matrix::CreateLookAt(cameraPos, Vector3::Zero, Vector3::Up);

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

	for (Mesh mesh : meshes)
	{
		entityx::Entity meshEntity = entities.Create();
		meshEntity.Assign<Mesh>(mesh);
		meshEntity.Assign<Transform>();
		meshEntity.Assign<ConstantRotation>(20.0f, 10.0f, 10.0f);
	}
}

void GameWorld::Update(float deltaTime)
{
	systems.update<RotateObjectSystem>(deltaTime);
	systems.update<RenderingSystem>(deltaTime);
}


