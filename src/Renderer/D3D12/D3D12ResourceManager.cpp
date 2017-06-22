#if DX12
#include "D3D12ResourceManager.h"

void D3D12ResourceManager::LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes)
{
}

MeshGeometryPtr D3D12ResourceManager::GetMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
}

TexturePtr D3D12ResourceManager::GetTexture(const std::string& path)
{
}

#endif