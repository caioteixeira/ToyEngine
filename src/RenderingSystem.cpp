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
		packet.viewMatrix = camera.get()->viewMatrix;

		//TODO: Add support to multiple cameras
		break;
	};

	entityx::ComponentHandle<Mesh> mesh;
	for(auto entity : es.entities_with_components(mesh, transform))
	{
		MeshElement element;
		auto meshPosition = transform->position;
		auto meshScale = transform->scale;
		auto meshRotation = transform->rotation;
		auto meshGeometry = mesh->geometry;
		auto material = mesh->material;
		auto buffer = mesh->perObjectBuffer;

		auto quat = Quaternion::CreateFromYawPitchRoll(meshRotation.x, meshRotation.y, meshRotation.z);

		element.worldTransform = Matrix::CreateScale(meshScale) * 
			Matrix::CreateFromQuaternion(quat) * Matrix::CreateTranslation(meshPosition);
		element.mesh = meshGeometry;
		element.material = material;
		element.constantBuffer = buffer;
		packet.meshes.push_back(element);
	}

	mRenderer->RenderFrame(packet);
}
