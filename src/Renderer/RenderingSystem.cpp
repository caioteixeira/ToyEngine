#include "RenderingSystem.h"
#include <iostream>
#include "FramePacket.h"
#include "../Transform.h"
#include "../Camera.h"
#include <easy/profiler.h>
#include "PointLight.h"
#include "imgui.h"

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

void RenderingSystem::ShowDebugUI(entityx::EntityManager& es)
{
    // FPS
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
    }


    // Light Data
    ImGui::Begin("LightData");
    ImGui::Text("Ambient Light");
    ImGui::InputFloat3("Ambient", reinterpret_cast<float*>(&mAmbientLight));
    entityx::ComponentHandle<Transform> transform;
    entityx::ComponentHandle<PointLight> pointLight;
    for (auto entity : es.entities_with_components(transform, pointLight))
    {
        ImGui::BeginGroup();
        ImGui::Text("PointLight");
        ImGui::PushID(&pointLight->diffuse);
        ImGui::InputFloat3("## Position", reinterpret_cast<float*>(&transform->position));
        ImGui::InputFloat3("## Color", reinterpret_cast<float *>(&pointLight->diffuse));
        ImGui::InputFloat("## InnerRadius", &pointLight->innerRadius);
        ImGui::InputFloat("## OuterRadius", &pointLight->outerRadius);
        ImGui::PopID();
        ImGui::EndGroup();
    }

    ImGui::End();
}

void RenderingSystem::Update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    EASY_FUNCTION();

    assert(mRenderer != nullptr);

    ShowDebugUI(es);

    FramePacket packet;
    entityx::ComponentHandle<Transform> transform;
    entityx::ComponentHandle<Camera> camera;
    for (auto entity : es.entities_with_components(transform, camera))
    {
        packet.viewProjMatrix = camera->view * camera->proj;
        packet.cameraPos = transform->position;

        //TODO: Add support to multiple cameras
        break;
    };

    entityx::ComponentHandle<Mesh> mesh;
    for (auto entity : es.entities_with_components(mesh, transform))
    {
        MeshElement element;
        const auto meshPosition = transform->position;
        const auto meshScale = transform->scale;
        const auto meshRotation = transform->rotation;
        const auto meshGeometry = mesh->geometry;
        const auto material = mesh->material;
        const auto buffer = mesh->perObjectBuffer;

        auto quat = Quaternion::CreateFromYawPitchRoll(meshRotation.x, meshRotation.y, meshRotation.z);
        quat.Normalize();

        element.worldTransform = Matrix::CreateScale(meshScale) *
            Matrix::CreateFromQuaternion(quat) * Matrix::CreateTranslation(meshPosition);
        element.mesh = meshGeometry;
        element.material = material;
        element.constantBuffer = buffer;
        packet.meshes.push_back(element);
    }

    packet.ambientLightColor = mAmbientLight;
    memset(&packet.pointLights, 0, sizeof PointLightData * MAX_POINT_LIGHTS);
    entityx::ComponentHandle<PointLight> pointLight;
    int i = 0;
    for (auto entity : es.entities_with_components(pointLight, transform))
    {
        assert(i < MAX_POINT_LIGHTS);

        packet.pointLights[i].mEnabled = true;
        packet.pointLights[i].mDiffuse = pointLight->diffuse;
        packet.pointLights[i].mInnerRadius = pointLight->innerRadius;
        packet.pointLights[i].mOuterRadius = pointLight->outerRadius;
        packet.pointLights[i].mPosition = transform->position;
        i++;
    }

    mRenderer->RenderFrame(packet);
}
