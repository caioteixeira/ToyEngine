#include "GraphicsResourcesManager.h"
#include "D3D12/D3D12ResourceManager.h"
#include "../AssetManagement/OBJModelLoader.h"

GraphicsResourcesManager::GraphicsResourcesManager(D3D12ResourceManager* resourceManager) 
: mResourceManager(resourceManager)
{
}

MeshGeometry GraphicsResourcesManager::LoadMeshGeometry(OBJModelLoader::MeshDesc& meshData) const
{
    return mResourceManager->LoadMeshGeometry(meshData);
}

MaterialPtr GraphicsResourcesManager::CreatePhongMaterial(OBJModelLoader::PhongMaterialDesc& desc) const
{
    return mResourceManager->CreatePhongMaterial(desc);
}

TextureHandle GraphicsResourcesManager::GetTexture(const std::string& path) const
{
    return mResourceManager->GetTexture(path);
}
