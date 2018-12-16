#include "GameWorld.h"
#include "Transform.h"
#include "AssetManagement/JsonUtils.h"
#include "FPSCameraSystem.h"
#include "DebugSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderingSystem.h"
#include "Graphics/GraphicsCore.h"

using namespace Engine::ECS;

GameWorld::GameWorld(): 
    mLevelLoader(entities)
{
    //FIXME: Refactor to use static method
    mLevelLoader.RegisterComponentLoader("mesh", 
        [this](entityx::Entity entity, auto& value){
        LoadMesh(entity, value);
    });
}

GameWorld::~GameWorld()
= default;

void GameWorld::Init()
{
    auto fpsCameraSystem = systems.Add<FPSCameraSystem>();
    auto renderingSystem = systems.Add<RenderingSystem>();
    systems.Add<DebugSystem>();
    systems.Configure();
}

void GameWorld::LoadMesh(entityx::Entity rootEntity, rapidjson::Value& value)
{
    auto resourceManager = Graphics::GetRenderer()->GetResourceManager();

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
