#include "GameWorld.h"
#include "Renderer/RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "Renderer/PointLight.h"
#include "JsonUtils.h"
#include "Math.h"
#include "FPSCameraSystem.h"

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
    entityx::Entity cameraEntity = entities.Create();

    const Vector3 cameraPos(30.0, 30.0, 100.0);
    cameraEntity.Assign<Transform>()->position = cameraPos;

    auto aspectRatio = static_cast<float>(1440) / 900;
    cameraEntity.Assign<Camera>(1.0f, 1000.0f, aspectRatio, 0.25f * Math::PI);

    auto fpsCameraSystem = systems.Add<FPSCameraSystem>();
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

        //TODO: Set parent transform to rootEntity, when implementing scene graph
    }
}

void GameWorld::LoadScene(const std::string& path)
{
    mLevelLoader.Load(path);
}

void GameWorld::Update(double deltaTime)
{
    systems.update<FPSCameraSystem>(deltaTime);
    systems.update<RenderingSystem>(deltaTime);
}
