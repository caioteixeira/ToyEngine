#pragma once
#include <entityx/entityx.h>


namespace Engine::ECS
{
	class GameWorld : public entityx::EntityX
	{
	public:
		GameWorld();
		~GameWorld();
		
		void Update(float deltaTime);
	};
}
