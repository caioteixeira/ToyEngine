#pragma once
#include "ECS/System.h"

namespace Engine
{
    class DebugSystem : public entityx::System<DebugSystem>
    {
    public:
        void Update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
    private:
    };
}
