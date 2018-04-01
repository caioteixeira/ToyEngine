#pragma once

#include "ECS/System.h"

struct Camera;
struct Transform;

namespace Engine
{
    class FPSCameraSystem : public entityx::System<FPSCameraSystem>
    {
    public:
        void Update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

    private:
        void UpdateCamera(entityx::ComponentHandle<Camera>& camera, entityx::ComponentHandle<Transform>& transform, entityx::TimeDelta dt);
    };

}