﻿#if DX11
#include "../Mesh.h"
#include "../ConstantBufferStructs.h"
#include "D3D11ResourceManager.h"
#include "D3D11GraphicsDevice.h"
#include "../Utils.h"

D3D11ResourceManager::D3D11ResourceManager(D3D11GraphicsDevice& device)
	: mDevice(device)
{
	
}

D3D11ResourceManager::~D3D11ResourceManager()
{

}

MeshGeometryPtr D3D11ResourceManager::GetMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
	auto it = mGeometryCache.find(path);
	if(it != mGeometryCache.end())
	{
		return it->second;
	}

	auto geo = LoadMeshGeometry(path, inputLayoutName);
	mGeometryCache.emplace(path, geo);
	return geo;
}

TexturePtr D3D11ResourceManager::GetTexture(const std::string& path)
{
	auto it = mTextureCache.find(path);
	if (it != mTextureCache.end())
	{
		return it->second;
	}

	auto texture = LoadTexture(path);

	if(texture != nullptr)
	{
		mTextureCache.emplace(path, texture);
	}
	return texture;
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

TexturePtr D3D11ResourceManager::LoadTexture(const std::string& path) const
{
	int width;
	int height;
	std::string finalPath("Assets/");
	finalPath = finalPath + path;
	auto graphicsTexture = mDevice.CreateTextureFromFile(finalPath.c_str(), width, height);

	if(graphicsTexture == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<Texture>(graphicsTexture, width, height);
}

MaterialPtr D3D11ResourceManager::CreateMaterial(Utils::MaterialDesc& desc)
{
	auto material = std::make_shared<Material>();
	material->ambientColor = desc.ambient;
	material->diffuseColor = desc.diffuse;
	material->specularColor = desc.specular;
	material->shininess = desc.shininess;

	//TODO: Properly set illumination properties
	material->SetProperty(Diffuse);

	//TODO: Load textures
	if(!desc.diffuseTexName.empty())
	{
		material->diffuseTexture = GetTexture(desc.diffuseTexName);

		if(material->diffuseTexture == nullptr)
		{
			return nullptr;
		}

		material->SetProperty(DiffuseTexture);
	}

	//TODO: Setup pipeline state
	auto pipelineState = std::make_shared<PipelineState>();
	pipelineState->inputLayout = GetInputLayout("positionnormaltexcoord");

	material->pipelineState = pipelineState;

	return material;
}

void D3D11ResourceManager::LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes)
{
	std::vector<Vertex> vertices = {};
	std::vector<Utils::SubmeshDesc> meshes = {};
	std::unordered_map<std::string, Utils::MaterialDesc> materialMap = {};

	Utils::LoadObjFile(path, meshes, materialMap);

	std::unordered_map<std::string, MaterialPtr> materials = {};
	for(auto& desc : materialMap)
	{
		auto material = CreateMaterial(desc.second);

		if(material != nullptr)
		{
			materials[desc.first] = material;
			SDL_Log("Renderer: Succesfully loaded a material");
		}
	}

	for(auto& meshData : meshes)
	{
		auto vertexBuffer = mDevice.CreateGraphicsBuffer(meshData.vertices.data(), meshData.vertices.size() * sizeof(Vertex),
			EBF_VertexBuffer, ECPUAF_Neither, EGBU_Immutable);
		auto indexBuffer = mDevice.CreateGraphicsBuffer(meshData.indices.data(), meshData.indices.size() * sizeof(uint32_t),
			EBF_IndexBuffer, ECPUAF_Neither, EGBU_Immutable);
		MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer, meshData.indices.size());

		const auto it = materials.find(meshData.materialName);
		if(it != materials.end())
		{
			auto material = materials[meshData.materialName];

			Mesh mesh;
			mesh.geometry = geo;
			mesh.material = material;
			mesh.perObjectBuffer = mDevice.CreateGraphicsBuffer(nullptr, sizeof(PerObjectConstants), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);;
			outMeshes.push_back(mesh);

			SDL_Log("Renderer: Succesfully loaded a mesh element");
		}
	}

	SDL_Log("Renderer: Succesfully loaded OBJ File");
}

MeshGeometryPtr D3D11ResourceManager::LoadMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
	std::vector<Vertex> vertices = {};
	std::vector<uint32_t> indices = {};
	Utils::LoadModel(path, vertices, indices);

	auto vertexBuffer = mDevice.CreateGraphicsBuffer(vertices.data(), vertices.size() * sizeof(Vertex), 
		EBF_VertexBuffer, ECPUAF_Neither, EGBU_Immutable);
	auto indexBuffer = mDevice.CreateGraphicsBuffer(indices.data(), indices.size() * sizeof(uint32_t),
		EBF_IndexBuffer, ECPUAF_Neither, EGBU_Immutable);
	
	MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer, indices.size());

	return geo;
}
#endif

