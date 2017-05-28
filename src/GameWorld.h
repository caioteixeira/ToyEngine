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

		void Init(std::shared_ptr<Renderer> renderer);
		void LoadObjLevel(const std::string& path);
		void Update(float deltaTime);
	private:
		std::shared_ptr<Renderer> mRenderer;
	};
}
