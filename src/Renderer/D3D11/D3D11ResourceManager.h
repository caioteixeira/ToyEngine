#pragma once

#if DX11
#include "D3D11GraphicsDevice.h"
#include <unordered_map>
#include "../MeshGeometry.h"

class D3D11ResourceManager
{
public:
	D3D11ResourceManager(D3D11GraphicsDevice& mDevice);
	~D3D11ResourceManager();

private:
	D3D11GraphicsDevice& mDevice;
	InputLayoutPtr GetInputLayout(const std::string& layoutName);
	void RegisterInputLayout(const std::string& layoutName, InputLayoutPtr layout);

	std::unordered_map<std::string, MeshGeometryPtr> mGeometryCache;
	std::unordered_map<std::string, InputLayoutPtr> mInputLayoutCache;
};

#endif
