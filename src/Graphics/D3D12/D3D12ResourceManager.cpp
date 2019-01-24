#include <unordered_map>
#include "D3D12ResourceManager.h"
#include "D3D12Device.h"
#include "../Mesh.h"
#include "../Vertex.h"
#include "d3dx12.h"
#include "../../EngineCore.h"
#include "WICTextureLoader12.h"
#include "D3D12CommandContextManager.h"
#include "../ConstantBufferStructs.h"

using namespace Engine;

D3D12ResourceManager::D3D12ResourceManager(D3D12GraphicsDevice* device)
    : mDevice(device)
{
}

D3D12ResourceManager::~D3D12ResourceManager()
{
}

MeshGeometry D3D12ResourceManager::LoadMeshGeometry(OBJModelLoader::MeshDesc& meshData)
{
    const auto vertexBuffer = CreateGraphicsBuffer("Vertex Buffer", meshData.vertices.size(), sizeof(Vertex),
                                                      meshData.vertices.data());
    const auto indexBuffer = CreateGraphicsBuffer("Index Buffer", meshData.indices.size(), sizeof(uint32_t),
                                                     meshData.indices.data());

    MeshGeometry geometry;
    geometry.VertexBuffer = vertexBuffer;
    geometry.IndexBuffer = indexBuffer;
    return geometry;
}

TextureHandle D3D12ResourceManager::GetTexture(const std::string& path)
{
    auto it = mTextureCache.find(path);
    if (it != mTextureCache.end())
    {
        return it->second;
    }

    auto texture = LoadTexture(path);
    if (texture != TextureHandle::NullHandle())
    {
        mTextureCache.emplace(path, texture);
    }
    return texture;
}

MaterialPtr D3D12ResourceManager::CreatePhongMaterial(OBJModelLoader::PhongMaterialDesc& desc)
{
    auto material = std::make_shared<Material>();

    MaterialConstants materialConstants;
    materialConstants.kd = desc.diffuse.ToVector3();
    materialConstants.ks = desc.specular.ToVector3();
    materialConstants.ka = desc.ambient.ToVector3();
    materialConstants.ns = desc.shininess;

    material->constantBuffer = CreateGraphicsBuffer("MaterialConstants", 1, sizeof MaterialConstants, &materialConstants);

    //TODO: Properly set illumination properties
    material->SetProperty(Diffuse);

    material->diffuseTexture = GetTexture(desc.diffuseTexName);
    if (material->diffuseTexture != TextureHandle::NullHandle())
    {
        material->SetProperty(DiffuseTexture);
    }

    material->pipelineState = GetPipelineState(material->properties);

    mMaterials[desc.name] = material;
    return material;
}

PipelineStateHandle D3D12ResourceManager::GetPipelineState(MaterialProperties properties)
{
    if(mPSOCache.count(properties) > 0)
    {
        return mPSOCache[properties];
    }

    PipelineState state;

    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    // Create root signature
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsConstantBufferView(0);
    slotRootParameter[2].InitAsConstantBufferView(1);
    slotRootParameter[3].InitAsConstantBufferView(2);

    auto samplers = GetStaticSamplers();
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
                                            (UINT)samplers.size(), samplers.data(),
                                            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    state.rootSignature = mDevice->CreateRootSignature(rootSigDesc);

    //Compile Shaders
    std::vector<D3D_SHADER_MACRO> defines = {};
    if (properties & DiffuseTexture)
    {
        const D3D_SHADER_MACRO define = { "DiffuseTexture", "1" };
        defines.push_back(define);
    }
    defines.push_back({ nullptr, nullptr });

    auto vertexShader = mDevice->CompileShaderFromFile(L"Shaders/Phong.hlsl", defines.data(), "VS", "vs_5_0");
    auto pixelShader = mDevice->CompileShaderFromFile(L"Shaders/Phong.hlsl", defines.data(), "PS", "ps_5_0");

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    //Create pipeline state
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

    //
    // PSO for opaque objects.
    //
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = {inputLayout.data(), (UINT)inputLayout.size()};
    psoDesc.pRootSignature = state.rootSignature.Get();
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()),
        vertexShader->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()),
        pixelShader->GetBufferSize()
    };

    auto rastState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rastState.CullMode = D3D12_CULL_MODE_NONE;
    psoDesc.RasterizerState = rastState;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = mDevice->GetBackBufferFormat();
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = mDevice->GetDepthStencilFormat();

    ThrowIfFailed(
        mDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&state.pipelineState)));

    PipelineStateHandle handle;
    handle.value = mPipelinesStates.size();
    mPipelinesStates.push_back(state);

    mPSOCache.emplace(properties, handle);
   
    return handle;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D3D12ResourceManager::GetStaticSamplers()
{
    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
        0, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
        1, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        2, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
        3, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
        4, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
        0.0f, // mipLODBias
        8); // maxAnisotropy

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
        5, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressW
        0.0f, // mipLODBias
        8); // maxAnisotropy

    return {
        pointWrap, pointClamp,
        linearWrap, linearClamp,
        anisotropicWrap, anisotropicClamp
    };
}

TextureHandle D3D12ResourceManager::CreateTextureFromFile(const char* inFileName)
{
    std::string fileStr(inFileName);
    const std::string extension = fileStr.substr(fileStr.find_last_of('.'));
    const size_t cSize = strlen(inFileName) + 1;

    size_t retCount;
    std::wstring wc(cSize, L'#');
    mbstowcs_s(&retCount, &wc[0], cSize, inFileName, _TRUNCATE);
    HRESULT hr = -1;

    std::unique_ptr<uint8_t[]> decodedData;
    D3D12_SUBRESOURCE_DATA subresource;

    Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
    if (extension == ".png" || extension == ".bmp" || extension == ".PNG" || extension == ".BMP")
    {
        hr = DirectX::LoadWICTextureFromFile(mDevice->GetDevice(), wc.c_str(), buffer.ReleaseAndGetAddressOf(),
            decodedData, subresource);
    }
    else
    {
        //TODO: Use logger class
        Logger::DebugLogError("ERROR: GraphicsDriver can only load images of type DDS, PNG, or BMP.");
    }

    if (hr != S_OK)
    {
        Logger::DebugLogError("Problem Creating Texture From File");
        return TextureHandle::NullHandle();
    }

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(buffer.Get(), 0, 1);

    // Create the GPU upload buffer.
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap;
    hr = mDevice->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadHeap.GetAddressOf())
    );
    ThrowIfFailed(hr, "ERROR: Failed to create upload buffer");

    auto context = mDevice->GetCommandContextManager()->AllocateContext();

    UpdateSubresources(context->GetCommandList(), buffer.Get(), uploadHeap.Get(),
        0, 0, 1, &subresource);
    context->TransitionResource(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    context->Finish(true);

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = mDevice->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);

    //Create SRV
    const CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = buffer->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = buffer->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    mDevice->GetDevice()->CreateShaderResourceView(buffer.Get(), &srvDesc, hDescriptor);

    //Fill texture object
    GraphicsResource resource;
    resource.buffer = buffer;
    resource.state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    GraphicsTexture texture;
    texture.resource = resource;
    texture.descriptor = std::move(descriptorHeap);

    const auto index = mTextures.size();
    mTextures.push_back(std::move(texture));

    const TextureHandle handle(index);
    return handle;
}

TextureHandle D3D12ResourceManager::LoadTexture(const std::string& path)
{
    if (path.empty())
    {
        return TextureHandle::NullHandle();
    }

    std::string finalPath("Assets/");
    finalPath = finalPath + path;
    const auto graphicsTexture = CreateTextureFromFile(finalPath.c_str());
    return graphicsTexture;
}

GraphicsTexture * D3D12ResourceManager::GetTexture(const TextureHandle handle)
{
    return &mTextures[handle.value];
}

PipelineState* D3D12ResourceManager::GetPipelineState(const PipelineStateHandle handle)
{
    return &mPipelinesStates[handle.value];
}

GraphicsBufferHandle D3D12ResourceManager::CreateGraphicsBuffer(const std::string& name, size_t numElements,
    SIZE_T elementSize, const void* initialData)
{
    GraphicsResource resource;
    resource.bufferSize = numElements * elementSize;
    resource.state = D3D12_RESOURCE_STATE_COMMON;

    //Allocate buffer
    D3D12_HEAP_PROPERTIES heapProperties;
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC desc = {};
    desc.Alignment = 0;
    desc.DepthOrArraySize = 1;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Height = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Width = (UINT64)resource.bufferSize;

    const auto result = mDevice->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
        resource.state, nullptr, IID_PPV_ARGS(&resource.buffer));
    ThrowIfFailed(result, "ERROR! Failed to initialize buffer");

    //Copies initial data
    if (initialData)
    {
        //Allocate buffer
        D3D12_HEAP_PROPERTIES UploadHeapProps;
        UploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        UploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        UploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        UploadHeapProps.CreationNodeMask = 1;
        UploadHeapProps.VisibleNodeMask = 1;

        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
        auto hr = mDevice->GetDevice()->CreateCommittedResource(&UploadHeapProps, D3D12_HEAP_FLAG_NONE,
            &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
            IID_PPV_ARGS(&uploadBuffer));
        ThrowIfFailed(hr);

        auto context = mDevice->GetCommandContextManager()->AllocateContext();
        void* dest;
        ThrowIfFailed(uploadBuffer->Map(0, nullptr, &dest));
        memcpy(dest, initialData, resource.bufferSize);
        uploadBuffer->Unmap(0, nullptr);

        context->TransitionResource(&resource, D3D12_RESOURCE_STATE_COPY_DEST);
        context->GetCommandList()->CopyBufferRegion(resource.buffer.Get(), 0, uploadBuffer.Get(), 0,
            resource.bufferSize);
        context->TransitionResource(&resource, D3D12_RESOURCE_STATE_GENERIC_READ);

        context->Finish(true);
    }

    GraphicsBuffer buffer;
    buffer.elementSize = elementSize;
    buffer.numElements = numElements;
    buffer.resource = resource;

    GraphicsBufferHandle handle;
    handle.value = mGraphicsBuffers.size();
    mGraphicsBuffers.push_back(buffer);
    return handle;
}

GraphicsBuffer* D3D12ResourceManager::GetGraphicsBuffer(const GraphicsBufferHandle handle)
{
    return &mGraphicsBuffers[handle.value];
}
