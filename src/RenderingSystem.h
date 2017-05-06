#pragma once
#include <entityx/System.h>

class RenderingSystem : public entityx::System<RenderingSystem>
{
public:
	RenderingSystem();
	~RenderingSystem();

	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
};

