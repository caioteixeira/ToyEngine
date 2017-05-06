#include "RenderingSystem.h"
#include <iostream>


RenderingSystem::RenderingSystem()
{
}


RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	std::cout << "Update Rendering System " << dt << std::endl;
}
