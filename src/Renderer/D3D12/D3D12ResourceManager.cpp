#include <unordered_map>
#include "D3D12ResourceManager.h"
#include "D3D12Device.h"
#include "../OBJModelLoader.h"
#include "../Mesh.h"
#include "../Vertex.h"
#include "d3dx12.h"
#include "../../EngineCore.h"

using namespace Engine;

D3D12ResourceManager::D3D12ResourceManager(D3D12GraphicsDevice* device)
    : mDevice(device)
{
}

D3D12ResourceManager::~D3D12ResourceManager()
{
}

void D3D12ResourceManager::LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes)
{
    std::vector<Vertex> vertices = {};
    std::vector<OBJModelLoader::SubmeshDesc> meshes = {};
    std::unordered_map<std::string, OBJModelLoader::MaterialDesc> materialMap = {};

    OBJModelLoader::LoadObjFile(path, meshes, materialMap);

    std::unordered_map<std::string, MaterialPtr> materials = {};
    for (auto& desc : materialMap)
    {
        auto& material = CreateMaterial(desc.second);

        if (material != nullptr)
        {
            materials[desc.first] = material;
            Logger::DebugLog("Renderer: Succesfully loaded a material");
        }
    }

    for (auto& meshData : meshes)
    {
        auto vertexBuffer = mDevice->CreateGraphicsBuffer("Vertex Buffer", meshData.vertices.size(), sizeof(Vertex),
                                                          meshData.vertices.data());
        auto indexBuffer = mDevice->CreateGraphicsBuffer("Index Buffer", meshData.indices.size(), sizeof(uint32_t),
                                                         meshData.indices.data());

        MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer,
                                                             static_cast<int>(meshData.indices.size()));

        if (materials.find(meshData.materialName) != materials.end())
        {
            auto& material = materials[meshData.materialName];

            Mesh mesh;
            mesh.geometry = geo;
            mesh.material = material;
            outMeshes.push_back(mesh);

            Logger::DebugLog("Renderer: Succesfully loaded a mesh element");
        }
    }

    Logger::DebugLog("Renderer: Succesfully loaded OBJ File");
}

MeshGeometryPtr D3D12ResourceManager::GetMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
    //TODO: Implement
    return nullptr;
}

TexturePtr D3D12ResourceManager::GetTexture(const std::string& path)
{
    auto it = mTextureCache.find(path);
    if (it != mTextureCache.end())
    {
        return it->second;
    }

    auto texture = LoadTexture(path);
    if (texture != nullptr)
    {
        mTextureCache.emplace(path, texture);
    }
    return texture;
}

MaterialPtr D3D12ResourceManager::CreateMaterial(OBJModelLoader::MaterialDesc& desc)
{
    auto material = std::make_shared<Material>();
    material->ambientColor = desc.ambient;
    material->diffuseColor = desc.diffuse;
    material->specularColor = desc.specular;
    material->shininess = desc.shininess;

    //TODO: Properly set illumination properties
    material->SetProperty(Diffuse);

    material->diffuseTexture = GetTexture(desc.diffuseTexName);
    if (material->diffuseTexture != nullptr)
    {
        material->SetProperty(DiffuseTexture);
    }

    material->pipelineState = GetPipelineState(material->properties);

    return material;
}

PipelineStatePtr D3D12ResourceManager::GetPipelineState(MaterialProperties properties)
{
    if(mPSOCache.count(properties) > 0)
    {
        return mPSOCache[properties];
    }

    auto state = std::make_shared<PipelineState>();

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

    state->rootSignature = mDevice->CreateRootSignature(rootSigDesc);

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
    psoDesc.pRootSignature = state->rootSignature.Get();
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
        mDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&state->pipelineState)));

    mPSOCache.emplace(properties, state);

    return state;
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

TexturePtr D3D12ResourceManager::LoadTexture(const std::string& path) const
{
    if (path.size() == 0)
    {
        return nullptr;
    }

    //TODO: Remove width and height from Texture class
    int width = -1;
    int height = -1;
    std::string finalPath("Assets/");
    finalPath = finalPath + path;
    auto graphicsTexture = mDevice->CreateTextureFromFile(finalPath.c_str(), width, height);

    if (graphicsTexture == nullptr)
    {
        return nullptr;
    }

    return std::make_shared<Texture>(graphicsTexture, width, height);
}
