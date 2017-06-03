#include "RotateObjectSystem.h"
#include "Transform.h"
#include "ConstantRotation.h"

void RotateObjectSystem::Update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<ConstantRotation> rotation;
	for (auto entity : entities.entities_with_components(transform, rotation))
	{
		transform->rotation += rotation->rotationBySecond * (dt / 1000.0f);
	};
}
