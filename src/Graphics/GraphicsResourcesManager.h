#pragma once
#include "MeshGeometry.h"
#include "Material.h"

namespace OBJModelLoader {
    struct PhongMaterialDesc;
    struct MeshDesc;
}

class D3D12ResourceManager;
namespace Engine
{
    class GraphicsResourcesManager
    {
    public:
        explicit GraphicsResourcesManager(D3D12ResourceManager* resourceManager);
        ~GraphicsResourcesManager() = default;
        MeshGeometryPtr LoadMeshGeometry(OBJModelLoader::MeshDesc& meshData) const;
        MaterialPtr CreatePhongMaterial(OBJModelLoader::PhongMaterialDesc& desc) const;
        TextureHandle GetTexture(const std::string& path) const;
    private:
        D3D12ResourceManager* mResourceManager = nullptr;
    };
}
