#pragma once

#if DX11
#include "../IResourceManager.h"
#include <unordered_map>
#include "../Mesh.h"
#include "../Texture.h"

class D3D11GraphicsDevice;
class D3D11ResourceManager : public IResourceManager
{
public:
	D3D11ResourceManager(D3D11GraphicsDevice& mDevice);
	~D3D11ResourceManager();
	void LoadObjFile(const std::string& path, std::vector<Mesh>& outMeshes);
	MeshGeometryPtr GetMeshGeometry(const std::string& path, const std::string& inputLayoutName);
	TexturePtr GetTexture(const std::string & path);
private:
	D3D11GraphicsDevice& mDevice;
	InputLayoutPtr GetInputLayout(const std::string& layoutName);
	void RegisterInputLayout(const std::string& layoutName, InputLayoutPtr layout);

	TexturePtr LoadTexture(const std::string& path) const;
	MeshGeometryPtr LoadMeshGeometry(const std::string& path, const std::string& inputLayoutName);

	std::unordered_map<std::string, MeshGeometryPtr> mGeometryCache;
	std::unordered_map<std::string, InputLayoutPtr> mInputLayoutCache;
	std::unordered_map<std::string, TexturePtr> mTextureCache;
};

#endif
