#include "../Mesh.h"
#if DX11
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
	mTextureCache.emplace(path, texture);
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
	auto graphicsTexture = mDevice.CreateTextureFromFile(path.c_str(), width, height);

	return std::make_shared<Texture>(graphicsTexture, width, height);
}

MaterialPtr D3D11ResourceManager::CreateMaterial(Utils::MaterialDesc desc)
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
		material->SetProperty(DiffuseTexture);
	}

	return material;
}

void D3D11ResourceManager::LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes)
{
	std::vector<Vertex> vertices = {};
	std::vector<Utils::SubmeshDesc> meshes = {};
	std::unordered_map<std::string, Utils::MaterialDesc> materialMap = {};

	Utils::LoadObjFile(path, vertices, meshes, materialMap);

	auto vertexBuffer = mDevice.CreateGraphicsBuffer(vertices.data(), vertices.size() * sizeof(Vertex),
		EBF_VertexBuffer, ECPUAF_Neither, EGBU_Immutable);

	std::unordered_map<std::string, MaterialPtr> materials = {};
	for(auto desc : materialMap)
	{
		auto material = CreateMaterial(desc.second);
		materials[desc.first] = material;
	}

	for(auto meshData : meshes)
	{
		auto indexBuffer = mDevice.CreateGraphicsBuffer(meshData.indices.data(), meshData.indices.size() * sizeof(int),
			EBF_IndexBuffer, ECPUAF_Neither, EGBU_Immutable);
		auto inputLayout = GetInputLayout("positionnormaltexcoord");
		MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer, meshData.indices.size(), inputLayout);
		auto material = materials[meshData.materialName];

		Mesh mesh(geo, material);
		outMeshes.push_back(mesh);
	}
}

MeshGeometryPtr D3D11ResourceManager::LoadMeshGeometry(const std::string& path, const std::string& inputLayoutName)
{
	std::vector<Vertex> vertices = {};
	std::vector<int> indices = {};
	Utils::LoadModel(path, vertices, indices);

	auto vertexBuffer = mDevice.CreateGraphicsBuffer(vertices.data(), vertices.size() * sizeof(Vertex), 
		EBF_VertexBuffer, ECPUAF_Neither, EGBU_Immutable);
	auto indexBuffer = mDevice.CreateGraphicsBuffer(indices.data(), indices.size() * sizeof(size_t), 
		EBF_IndexBuffer, ECPUAF_Neither, EGBU_Immutable);
	auto inputLayout = GetInputLayout(inputLayoutName);
	
	MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer, indices.size(), inputLayout);

	return geo;
}
#endif

