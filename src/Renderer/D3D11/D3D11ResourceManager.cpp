#if DX11
#include "D3D11ResourceManager.h"
#include "../Utils.h"

D3D11ResourceManager::D3D11ResourceManager(D3D11GraphicsDevice& device)
	: mDevice(device)
{
}

MeshGeometryPtr D3D11ResourceManager::GetMeshGeometry(std::string& path)
{
	auto it = mGeometryCache.find(path);
	if(it != mGeometryCache.end())
	{
		return it->second;
	}

	auto geo = LoadMeshGeometry(path);
	mGeometryCache.emplace(path, geo);
	return geo;
}

InputLayoutPtr D3D11ResourceManager::GetInputLayout(const std::string& layoutName)
{
	auto ret = mInputLayoutCache.find(layoutName);
	return ret != mInputLayoutCache.end() ? ret->second : nullptr;
}

void D3D11ResourceManager::RegisterInputLayout(const std::string& layoutName, InputLayoutPtr layout)
{
	mInputLayoutCache[layoutName] = layout;
}

MeshGeometryPtr D3D11ResourceManager::LoadMeshGeometry(std::string& path)
{
	std::vector<Vertex> vertices = {};
	std::vector<int> indices = {};
	Utils::LoadModel(path, vertices, indices);

	return nullptr;
}
#endif

