#pragma once

#if DX12
#include <array>
#include "../IResourceManager.h"
#include "../Utils.h"
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
	MaterialPtr CreateMaterial(Utils::MaterialDesc& desc);
	PipelineStatePtr GetPipelineState(MaterialProperties properties);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	//TODO: Create a pipeline cache for multiple bindings
	PipelineStatePtr mDefaultPipeline;
	D3D12GraphicsDevice* mDevice;
};
#endif