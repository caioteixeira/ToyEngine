#if DX11
#include "D3D11ResourceManager.h"
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

InputLayoutPtr D3D11ResourceManager::GetInputLayout(const std::string& layoutName)
{
	auto ret = mInputLayoutCache.find(layoutName);
	return ret != mInputLayoutCache.end() ? ret->second : nullptr;
}

void D3D11ResourceManager::RegisterInputLayout(const std::string& layoutName, InputLayoutPtr layout)
{
	mInputLayoutCache[layoutName] = layout;
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
	
	MeshGeometryPtr geo = std::make_shared<MeshGeometry>(vertexBuffer, indexBuffer, inputLayout);

	return nullptr;
}
#endif

