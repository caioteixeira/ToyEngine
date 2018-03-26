#include "GameWorld.h"
#include "Renderer/RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "Renderer/PointLight.h"
#include "JsonUtils.h"

using namespace Engine::ECS;

static void LoadTransform(entityx::Entity entity, rapidjson::Value& value)
{
    Vector3 position;
    GetVectorFromJSON(value, "position", position);

    auto transform = entity.Assign<Transform>();
    transform->position = position;
}

GameWorld::GameWorld(): 
    mLevelLoader(entities)
{
    mLevelLoader.RegisterComponentLoader("transform", LoadTransform);

    //FIXME: Refactor to use static method
    mLevelLoader.RegisterComponentLoader("mesh", 
        [this](entityx::Entity entity, auto& value){
        LoadMesh(entity, value);
    });
}

GameWorld::~GameWorld()
= default;

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

void GameWorld::LoadMesh(entityx::Entity rootEntity, rapidjson::Value& value)
{
    auto resourceManager = mRenderer->GetResourceManager();

    std::vector<Mesh> meshes;
    std::string modelPath;

    if (!GetStringFromJSON(value, "modelPath", modelPath))
    {
        return;
    }

    resourceManager->LoadObjFile(modelPath, meshes);

    for (Mesh& mesh : meshes)
    {
        entityx::Entity meshEntity = entities.Create();
        meshEntity.Assign<Mesh>(mesh);
        meshEntity.Assign<Transform>();

        //TODO: Set parent transform to rootEntity
    }
}

void GameWorld::LoadScene(const std::string& path)
{
    mLevelLoader.Load(path);
}

void GameWorld::Update(double deltaTime)
{
    systems.update<RenderingSystem>(deltaTime);
}
