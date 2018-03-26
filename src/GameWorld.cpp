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

static void LoadPointLight(entityx::Entity entity, rapidjson::Value& value)
{
    Vector3 diffuse;
    GetVectorFromJSON(value, "diffuse", diffuse);

    float innerRadius;
    GetFloatFromJSON(value, "innerRadius", innerRadius);

    float outerRadius;
    GetFloatFromJSON(value, "outerRadius", outerRadius);

    auto pointLight = entity.Assign<PointLight>();
    pointLight->diffuse = diffuse;
    pointLight->innerRadius = innerRadius;
    pointLight->outerRadius = outerRadius;
}

GameWorld::GameWorld(): 
    mLevelLoader(entities)
{
    mLevelLoader.RegisterComponentLoader("transform", LoadTransform);
    mLevelLoader.RegisterComponentLoader("pointLight", LoadPointLight);

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
