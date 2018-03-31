
#pragma once

#include <functional>
#include <string>
#include "ECS/entityx.h"
#include "document.h"

namespace Engine
{
    typedef std::function<void (entityx::Entity, rapidjson::Value&)> ComponentLoaderFunc;

    class LevelLoader
    {
    public:
        LevelLoader(entityx::EntityManager& manager);
        void Load(const std::string& filename);
        void RegisterComponentLoader(std::string, ComponentLoaderFunc);
    private:
        void LoadEntities(rapidjson::GenericValue<rapidjson::UTF8<>>::Array& array);

        entityx::EntityManager& mEntityManager;
        std::unordered_map<std::string, ComponentLoaderFunc> mLoaderMap;
    };
}
