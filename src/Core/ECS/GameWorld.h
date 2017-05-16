#pragma once
#include "quick.h"


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
