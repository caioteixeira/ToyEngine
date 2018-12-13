#pragma once

#include <array>
#include "../IResourceManager.h"
#include "../../AssetManagement/OBJModelLoader.h"
#include "d3dx12.h"

class D3D12GraphicsDevice;

class D3D12ResourceManager : IResourceManager
{
public:
    D3D12ResourceManager(D3D12GraphicsDevice* device);
    ~D3D12ResourceManager();
    MeshGeometryPtr LoadMeshGeometry(OBJModelLoader::SubmeshDesc meshData) const;
    MaterialPtr CreatePhongMaterial(OBJModelLoader::MaterialDesc& desc);
    TextureHandle GetTexture(const std::string& path);

private:
    friend class D3D12Renderer;

    PipelineStateHandle GetPipelineState(MaterialProperties properties);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
    TextureHandle CreateTextureFromFile(const char* inFileName);
    TextureHandle LoadTexture(const std::string& path);
    GraphicsTexture* GetTexture(const TextureHandle handle);
    PipelineState* GetPipelineState(const PipelineStateHandle handle);

    //TODO: Create a pipeline cache for multiple bindings
    D3D12GraphicsDevice* mDevice;

    std::unordered_map<std::string, TextureHandle> mTextureCache;
    std::unordered_map<MaterialProperties, PipelineStateHandle> mPSOCache;

    std::vector<GraphicsTexture> mTextures;
    std::vector<PipelineState> mPipelinesStates;
    std::unordered_map<std::string, MaterialPtr> mMaterials;
};
