#if DX12
#include <unordered_map>
#include "D3D12ResourceManager.h"
#include "D3D12Device.h"
#include "../Utils.h"
#include "../Mesh.h"
#include "../Vertex.h"
#include "d3dx12.h"

D3D12ResourceManager::D3D12ResourceManager(D3D12GraphicsDevice * device)
	: mDevice(device)
{
}

D3D12ResourceManager::~D3D12ResourceManager()
{
}

void D3D12ResourceManager::LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes)
{
	std::vector<Vertex> vertices = {};
	std::vector<Utils::SubmeshDesc> meshes = {};
	std::unordered_map<std::string, Utils::MaterialDesc> materialMap = {};

	Utils::LoadObjFile(path, meshes, materialMap);

	std::unordered_map<std::string, MaterialPtr> materials = {};
	for (auto& desc : materialMap)
	{
		auto material = CreateMaterial(desc.second);
		materials[desc.first] = material;

		SDL_Log("Renderer: Succesfully loaded a material");
	}

	for (auto& meshData : meshes)
	{
		auto vertexBuffer = mDevice->CreateGraphicsBuffer("Vertex Buffer", meshData.vertices.size(), sizeof(Vertex), meshData.vertices.data());
		auto indexBuffer = mDevice->CreateGraphicsBuffer("Index Buffer", meshData.indices.size(), sizeof(size_t), meshData.indices.data());

		MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer, meshData.indices.size());
		//auto material = materials[meshData.materialName];

		Mesh mesh;
		mesh.geometry = geo;
		// mesh.material = material;
		outMeshes.push_back(mesh);

		SDL_Log("Renderer: Succesfully loaded a mesh element");
	}

	SDL_Log("Renderer: Succesfully loaded OBJ File");
}

MeshGeometryPtr D3D12ResourceManager::GetMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
	//TODO: Implement
	return nullptr;
}

TexturePtr D3D12ResourceManager::GetTexture(const std::string& path)
{
	//TODO: Implement
	return nullptr;
}

MaterialPtr D3D12ResourceManager::CreateMaterial(Utils::MaterialDesc& desc)
{
	auto material = std::make_shared<Material>();
	material->ambientColor = desc.ambient;
	material->diffuseColor = desc.diffuse;
	material->specularColor = desc.specular;
	material->shininess = desc.shininess;

	//TODO: Properly set illumination properties
	material->SetProperty(Diffuse);

	return material;
}

PipelineStatePtr D3D12ResourceManager::GetPipelineState(MaterialProperties properties)
{
	auto state = std::make_shared<PipelineState>();

	// Create root signature
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	auto samplers = GetStaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)samplers.size(), samplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	state->rootSignature = mDevice->CreateRootSignature(rootSigDesc);

	//Compile Shaders
	auto vertexShader = mDevice->CompileShaderFromFile(L"Assets/BasicMesh.hlsl", nullptr, "VS", "vs_5_0");
	auto pixelShader = mDevice->CompileShaderFromFile(L"Assets/BasicMesh.hlsl", nullptr, "PS", "vs_5_0");

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//Create pipeline state
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = {inputLayout.data(), (UINT)inputLayout.size() };
	opaquePsoDesc.pRootSignature = state->rootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()),
		vertexShader->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()),
		pixelShader->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	//TODO: Get format from a unique place
	opaquePsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	opaquePsoDesc.SampleDesc.Count = 1;
	opaquePsoDesc.SampleDesc.Quality = 0;
	//TODO: Get format from a unique place
	opaquePsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	ThrowIfFailed(mDevice->GetDevice()->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&state->pipelineState)));

	return state;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D3D12ResourceManager::GetStaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

#endif