#pragma once

#include <array>
#include "../IResourceManager.h"
#include "../OBJModelLoader.h"
#include "d3dx12.h"

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
	MaterialPtr CreateMaterial(OBJModelLoader::MaterialDesc& desc);
	PipelineStatePtr GetPipelineState(MaterialProperties properties);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	TexturePtr LoadTexture(const std::string& path) const;

	//TODO: Create a pipeline cache for multiple bindings
	D3D12GraphicsDevice* mDevice;

	std::unordered_map<std::string, TexturePtr> mTextureCache;
};