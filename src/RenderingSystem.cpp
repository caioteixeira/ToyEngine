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

	entityx::ComponentHandle<Mesh> mesh;
	for(auto entity : es.entities_with_components(mesh))
	{
		MeshElement element;
		element.mesh = mesh.get()->GetGeometry();
		element.material = mesh.get()->GetMaterial();
		packet.meshes.push_back(element);
	}

	mRenderer->RenderFrame(packet);
}
