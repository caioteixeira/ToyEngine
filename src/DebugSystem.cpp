#include "DebugSystem.h"
#include "../External/imgui/imgui.h"
#include "Transform.h"
#include "Graphics/PointLight.h"
#include "Graphics/Mesh.h"
#include "NameComponent.h"

void DebugSystem::Update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    // FPS
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
    }

    //Mesh transforms
    ImGui::Begin("Meshes");
    entityx::ComponentHandle<Transform> transform;
    entityx::ComponentHandle<MeshRenderer> mesh;
    entityx::ComponentHandle<NameComponent> name;
    for (auto entity : es.entities_with_components(transform, mesh, name))
    {
        ImGui::BeginGroup();
        ImGui::Text(name->name.c_str());
        ImGui::PushID(name->name.c_str());
        ImGui::Text("Position");
        ImGui::InputFloat3("## position", reinterpret_cast<float *>(&transform->position));
        ImGui::Text("Scale");
        ImGui::InputFloat3("## scale", reinterpret_cast<float *>(&transform->scale));
        ImGui::PopID();
        ImGui::EndGroup();
    }

    ImGui::End();
}
