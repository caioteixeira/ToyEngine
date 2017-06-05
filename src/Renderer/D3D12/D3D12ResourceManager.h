#pragma once

#if DX12
#include "../IResourceManager.h"

class D3D12ResourceManager : IResourceManager
{
public:
	void LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes);
	MeshGeometryPtr GetMeshGeometry(const std::string& path, const std::string& inputLayoutName);
	TexturePtr GetTexture(const std::string& path);
};
#endif