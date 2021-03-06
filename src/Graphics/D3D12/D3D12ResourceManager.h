﻿#pragma once

#include <array>
#include "../../AssetManagement/OBJModelLoader.h"
#include "d3dx12.h"
#include "../MeshGeometry.h"
#include "../Types.h"

class D3D12GraphicsDevice;

class D3D12ResourceManager
{
public:
    explicit D3D12ResourceManager(D3D12GraphicsDevice* device);
    ~D3D12ResourceManager();
    MeshGeometry LoadMeshGeometry(OBJModelLoader::MeshDesc& meshData);
    MaterialPtr CreatePhongMaterial(OBJModelLoader::PhongMaterialDesc& desc);
    TextureHandle GetTexture(const std::string& path);

private:
    friend class D3D12Renderer;

    PipelineStateHandle GetPipelineState(MaterialProperties properties);
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
    TextureHandle CreateTextureFromFile(const char* inFileName);
    TextureHandle LoadTexture(const std::string& path);
    GraphicsTexture* GetTexture(const TextureHandle handle);
    PipelineState* GetPipelineState(const PipelineStateHandle handle);

    GraphicsBufferHandle CreateGraphicsBuffer(const std::string& name, size_t numElements, SIZE_T elementSize,
        const void* initialData);
    GraphicsBuffer* GetGraphicsBuffer(const GraphicsBufferHandle handle);

    //TODO: Create a pipeline cache for multiple bindings
    D3D12GraphicsDevice* mDevice;

    std::unordered_map<std::string, TextureHandle> mTextureCache;
    std::unordered_map<MaterialProperties, PipelineStateHandle> mPSOCache;

    std::vector<GraphicsTexture> mTextures;
    std::vector<PipelineState> mPipelinesStates;
    std::vector<GraphicsBuffer> mGraphicsBuffers;

    std::unordered_map<std::string, MaterialPtr> mMaterials;
};
