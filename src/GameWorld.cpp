#include "GameWorld.h"
#include "Renderer/RenderingSystem.h"
#include "Transform.h"
#include "Camera.h"
#include "Renderer/PointLight.h"
#include "AssetManagement/JsonUtils.h"
#include "Math.h"
#include "FPSCameraSystem.h"
#include "DebugSystem.h"

using namespace Engine::ECS;

static void LoadTransform(entityx::Entity entity, rapidjson::Value& value)
{
    auto transform = entity.Assign<Transform>();

    Vector3 position;
    GetVectorFromJSON(value, "position", position);
    transform->position = position;

    Vector3 scale;
    if(GetVectorFromJSON(value, "scale", scale))
    {
        transform->scale = scale;
    }
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

static void LoadCamera(entityx::Entity entity, rapidjson::Value& value)
{
    const auto windowWidth = CVar::Get("windowWidth")->intValue;
    const auto windowHeight = CVar::Get("windowHeight")->intValue;

    float nearPlane;
    GetFloatFromJSON(value, "nearPlane", nearPlane);

    float farPlane;
    GetFloatFromJSON(value, "farPlane", farPlane);

    auto aspectRatio = static_cast<float>(windowWidth) / windowHeight;
    entity.Assign<Camera>(nearPlane, farPlane, aspectRatio, 0.25f * Math::PI);
}

GameWorld::GameWorld(): 
    mLevelLoader(entities)
{
    mLevelLoader.RegisterComponentLoader("transform", LoadTransform);
    mLevelLoader.RegisterComponentLoader("pointLight", LoadPointLight);
    mLevelLoader.RegisterComponentLoader("camera", LoadCamera);

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
    auto fpsCameraSystem = systems.Add<FPSCameraSystem>();
    auto renderingSystem = systems.Add<RenderingSystem>();
    systems.Add<DebugSystem>();
    systems.Configure();
    renderingSystem->SetRenderer(renderer);
    mRenderer = renderer;
}

void GameWorld::LoadMesh(entityx::Entity rootEntity, rapidjson::Value& value)
{
    auto resourceManager = mRenderer->GetResourceManager();

    std::string modelPath;

    if (!GetStringFromJSON(value, "modelPath", modelPath))
    {
        return;
    }

    std::vector<OBJModelLoader::PhongMaterialDesc> materialMap;
    std::vector<OBJModelLoader::MeshDesc> subMeshDescriptors;
    LoadObjFile(modelPath, subMeshDescriptors, materialMap);

    std::unordered_map<std::string, MaterialPtr> materials;
    for (auto material : materialMap)
    {
        const auto materialHandle = resourceManager->CreatePhongMaterial(material);
        materials[material.name] = materialHandle;
    }

    for (auto& descriptor : subMeshDescriptors)
    {
        auto mesh = resourceManager->LoadMeshGeometry(descriptor);

        entityx::Entity meshEntity = entities.Create();
        meshEntity.Assign<MeshRenderer>(mesh, materials[descriptor.materialName]);
        meshEntity.Assign<Transform>();
    }
}

void GameWorld::LoadScene(const std::string& path)
{
    mLevelLoader.Load(path);
}

void GameWorld::Update(double deltaTime)
{
    systems.update<FPSCameraSystem>(deltaTime);
    systems.update<DebugSystem>(deltaTime);
    systems.update<RenderingSystem>(deltaTime);
}
