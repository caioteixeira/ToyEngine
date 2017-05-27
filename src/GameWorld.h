#pragma once
#include "ECS/entityx.h"
#include "Renderer/Renderer.h"


namespace Engine::ECS
{
	class GameWorld : public entityx::EntityX
	{
	public:
		GameWorld();
		~GameWorld();
		void SetRenderer(std::shared_ptr<Renderer> renderer);

		void Update(float deltaTime);
	};
}
