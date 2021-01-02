#pragma once
#include "Renderer.h"
#include "../ECS/System.h"
#include <functional>

class RenderingSystem : public entityx::System<RenderingSystem>
{
public:
    RenderingSystem();
    ~RenderingSystem();

    void Update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
private:
    void ShowDebugUI(entityx::EntityManager& es);

    Renderer* mRenderer = nullptr;
    Vector3 mAmbientLight = Vector3(0.5f, 0.5f, 0.5f);
};
