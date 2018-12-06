#include "IResourceManager.h"

void IResourceManager::LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes)
{
}

MeshGeometryPtr IResourceManager::GetMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
    return nullptr;
}

TextureHandle IResourceManager::GetTexture(const std::string& path)
{
    return TextureHandle::NullHandle();
}
