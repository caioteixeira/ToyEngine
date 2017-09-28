#include "RenderingSystem.h"
#include <iostream>
#include "Renderer/FramePacket.h"
#include "Transform.h"
#include "Camera.h"
#include "Core/imgui/imgui.h"
#include "Core/imgui/imgui_impl_dx11.h"
#include <easy/profiler.h>

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

void ShowImGUISample()
{
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		ImGui::Text("Hello, world!");;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

void RenderingSystem::Update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	EASY_FUNCTION();

	//ImGui_ImplDX11_NewFrame();

	assert(mRenderer != nullptr);

	//ShowImGUISample();

	FramePacket packet;
	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<Camera> camera;
	for(auto entity : es.entities_with_components(transform, camera))
	{
		auto viewMatrix = Matrix::CreateLookAt(transform->position, Vector3::Zero, Vector3::Up);
		packet.viewMatrix = viewMatrix;

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
		quat.Normalize();

		element.worldTransform = Matrix::CreateScale(meshScale) * 
			Matrix::CreateFromQuaternion(quat) * Matrix::CreateTranslation(meshPosition);
		element.mesh = meshGeometry;
		element.material = material;
		element.constantBuffer = buffer;
		packet.meshes.push_back(element);
	}

	mRenderer->RenderFrame(packet);
}
