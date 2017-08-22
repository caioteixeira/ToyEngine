#pragma once

#if DX12
#include "../IResourceManager.h"

class D3D12GraphicsDevice;

class D3D12ResourceManager : IResourceManager
{
public:
	D3D12ResourceManager(D3D12GraphicsDevice* device);
	~D3D12ResourceManager();
	void LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes);
	MeshGeometryPtr GetMeshGeometry(const std::string& path, const std::string& inputLayoutName);
	TexturePtr GetTexture(const std::string& path);

private:
	D3D12GraphicsDevice* mDevice;
};
#endif