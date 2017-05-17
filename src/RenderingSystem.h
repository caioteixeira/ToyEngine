#pragma once
#include "Renderer/Renderer.h"
#include "Core/ECS/System.h"

class RenderingSystem : public entityx::System<RenderingSystem>
{
public:
	RenderingSystem();
	~RenderingSystem();

	void Update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
private:
//	Renderer& mRenderer;
};

