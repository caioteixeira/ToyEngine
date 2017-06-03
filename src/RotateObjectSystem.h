#pragma once
#include "ECS/System.h"

class RotateObjectSystem : public entityx::System<RotateObjectSystem>
{
public:
	void Update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
};
