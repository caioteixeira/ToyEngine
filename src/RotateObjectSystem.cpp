#include "RotateObjectSystem.h"
#include "Transform.h"
#include "ConstantRotation.h"
#include <easy/profiler.h>

void RotateObjectSystem::Update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	EASY_FUNCTION();

	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<ConstantRotation> rotation;
	for (auto entity : entities.entities_with_components(transform, rotation))
	{
		transform->rotation += rotation->rotationBySecond * (dt);
	};
}
