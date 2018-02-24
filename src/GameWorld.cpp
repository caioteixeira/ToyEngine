#include "GameWorld.h"
#include "Renderer/RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "Renderer/PointLight.h"

using namespace Engine::ECS;

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
}

void GameWorld::Init(std::shared_ptr<Renderer> renderer)
{
    //TODO: Implement level Loader

    entityx::Entity cameraEntity = entities.Create();

    const Vector3 cameraPos(30.0, 30.0, 100.0);
    cameraEntity.Assign<Transform>()->position = cameraPos;
    cameraEntity.Assign<Camera>();

    auto lightEntity = entities.Create();
    auto point1transform = lightEntity.Assign<Transform>();
    point1transform->position = Vector3(400.0f, 100.0f, 0.0f);
    auto pointLight1 = lightEntity.Assign<PointLight>();
    pointLight1->diffuse = Vector3(1.0f, 0.1f, 1.0f);
    pointLight1->innerRadius = 100.0f;
    pointLight1->outerRadius = 400.0f;

    auto lightEntity2 = entities.Create();
    auto point2transform = lightEntity2.Assign<Transform>();
    point2transform->position = Vector3(100.0f, 100.0f, 0.0f);
    auto pointLight2 = lightEntity2.Assign<PointLight>();
    pointLight2->diffuse = Vector3(0.01f, 1.0f, 1.0f);
    pointLight2->innerRadius = 100.0f;
    pointLight2->outerRadius = 600.0f;

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
