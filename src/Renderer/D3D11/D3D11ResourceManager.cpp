#if DX11
#include "D3D11ResourceManager.h"

D3D11ResourceManager::D3D11ResourceManager(D3D11GraphicsDevice& device)
	: mDevice(device)
{
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
#endif

