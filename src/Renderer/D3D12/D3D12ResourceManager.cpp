#if DX12
#include <unordered_map>
#include "D3D12ResourceManager.h"
#include "D3D12Device.h"
#include "../Utils.h"
#include "../Mesh.h"
#include "../Vertex.h"

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

	/*std::unordered_map<std::string, MaterialPtr> materials = {};
	for (auto& desc : materialMap)
	{
		auto material = CreateMaterial(desc.second);
		materials[desc.first] = material;

		SDL_Log("Renderer: Succesfully loaded a material");
	}*/

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

#endif