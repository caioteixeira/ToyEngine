#include "LevelLoader.h"
#include "../EngineCore.h"
#include <fstream>
#include "../NameComponent.h"
#include "../Transform.h"
#include "JsonUtils.h"
#include "../Graphics/PointLight.h"
#include "../Camera.h"
#undef GetObject

Engine::LevelLoader::LevelLoader(entityx::EntityManager& manager)
    : mEntityManager(manager)
{
    InitDefaultLoaders();
}

static void LoadTransform(entityx::Entity entity, rapidjson::Value& value)
{
    auto transform = entity.Assign<Transform>();

    Vector3 position;
    GetVectorFromJSON(value, "position", position);
    transform->position = position;

    Vector3 scale;
    if (GetVectorFromJSON(value, "scale", scale))
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
    const auto windowWidth = Engine::CVar::Get("windowWidth")->intValue;
    const auto windowHeight = Engine::CVar::Get("windowHeight")->intValue;

    float nearPlane;
    GetFloatFromJSON(value, "nearPlane", nearPlane);

    float farPlane;
    GetFloatFromJSON(value, "farPlane", farPlane);

    auto aspectRatio = static_cast<float>(windowWidth) / windowHeight;
    entity.Assign<Camera>(nearPlane, farPlane, aspectRatio, 0.25f * Math::PI);
}


void Engine::LevelLoader::Load(const std::string& filename)
{
    Logger::DebugLog(filename.c_str());

    std::ifstream sceneFile(filename);
    if (!sceneFile.is_open())
    {
        Logger::DebugLogError("Failed to open scene file!");
        return;
    }

    std::string json;
    sceneFile.seekg(0, std::ios::end);
    json.reserve(sceneFile.tellg());
    sceneFile.seekg(0, std::ios::beg);
    json.assign((std::istreambuf_iterator<char>(sceneFile)), std::istreambuf_iterator<char>());

    rapidjson::Document document;
    if (document.Parse(json.c_str()).HasParseError())
    {
        Logger::DebugLogError("Failed to parse scene json!");
    }

    for(auto itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        if(itr->value.IsArray() && std::strcmp(itr->name.GetString(), "entities") == 0)
        {
            auto array = itr->value.GetArray();

            LoadEntities(array);
        }
    }
}

void Engine::LevelLoader::LoadEntities(rapidjson::GenericValue<rapidjson::UTF8<>>::Array& array)
{
    for (auto entityItr = array.Begin(); entityItr != array.End(); ++entityItr)
    {
        auto entity = mEntityManager.Create();
        
        for (auto propertyItr = entityItr->MemberBegin(); propertyItr != entityItr->MemberEnd(); ++propertyItr)
        {
            if(std::strcmp(propertyItr->name.GetString(), "name") == 0)
            {
                auto nameComp = entity.Assign<NameComponent>();
                const auto entityName = propertyItr->value.GetString();
                nameComp->name = entityName;

                continue;
            }

            if(std::strcmp(propertyItr->name.GetString(), "components") == 0 && propertyItr->value.IsArray())
            {
                auto componentArray = propertyItr->value.GetArray();
                for (auto componentItr = componentArray.Begin(); componentItr < componentArray.End(); ++componentItr)
                {
                    const auto typeMember = componentItr->FindMember("type");
                    if(typeMember == componentItr->MemberEnd())
                    {
                        Logger::DebugLogError("Could not find component type!");
                    }

                    const auto type = typeMember->value.GetString();
                    if(mLoaderMap.count(type) == 0)
                    {
                        Logger::DebugLogError("Could not find loader for type %s!", type);
                    }

                    auto& loader = mLoaderMap[type];
                    loader(entity, componentItr->GetObject()["properties"]);
                }
            }
        }
    }
}

void Engine::LevelLoader::InitDefaultLoaders()
{
    RegisterComponentLoader("transform", LoadTransform);
    RegisterComponentLoader("pointLight", LoadPointLight);
    RegisterComponentLoader("camera", LoadCamera);
}

void Engine::LevelLoader::RegisterComponentLoader(std::string componentType, ComponentLoaderFunc componentLoader)
{
    mLoaderMap.emplace(componentType, componentLoader);
}
