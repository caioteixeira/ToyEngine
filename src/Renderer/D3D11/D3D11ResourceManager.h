#pragma once
#include "../Utils.h"

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
	InputLayoutPtr GetInputLayout(const std::string& layoutName);
	void RegisterInputLayout(const std::string& layoutName, InputLayoutPtr layout);
private:
	D3D11GraphicsDevice& mDevice;

	TexturePtr LoadTexture(const std::string& path) const;
	MaterialPtr CreateMaterial(Utils::MaterialDesc& desc);
	MeshGeometryPtr LoadMeshGeometry(const std::string& path, const std::string& inputLayoutName);

	std::unordered_map<std::string, MeshGeometryPtr> mGeometryCache;
	std::unordered_map<std::string, InputLayoutPtr> mInputLayoutCache;
	std::unordered_map<std::string, TexturePtr> mTextureCache;
};

#endif
