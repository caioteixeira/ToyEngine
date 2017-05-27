#pragma once
#include "Renderer/Renderer.h"
#include "ECS/System.h"
#include <functional>

class RenderingSystem : public entityx::System<RenderingSystem>
{
public:
	RenderingSystem();
	~RenderingSystem();
	void SetRenderer(std::shared_ptr<Renderer> renderer);

	void Update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
private:
	std::shared_ptr<Renderer> mRenderer = nullptr;
};

