#include "RenderingSystem.h"
#include <iostream>
#include "Renderer/FramePacket.h"
#include "Transform.h"
#include "Camera.h"

RenderingSystem::RenderingSystem()
{
}


RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::SetRenderer(std::shared_ptr<Renderer> renderer)
{
	mRenderer = renderer;
}

void RenderingSystem::Update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	std::cout << "Update Rendering System " << dt << std::endl;

	if(mRenderer == nullptr)
	{
		std::cerr << "Renderer is undefined" << std::endl;
		return;
	}

	FramePacket packet;
	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<Camera> camera;
	for(auto entity : es.entities_with_components(transform, camera))
	{
		packet.viewMatrix = Matrix::CreateLookAt(transform.get()->position , Vector3::Zero, Vector3::Up);
		break;
	};

	mRenderer->RenderFrame(packet);
}
