#pragma once

#if DX11
#include "D3D11GraphicsDevice.h"

class D3D11ResourceManager
{
public:
	D3D11ResourceManager(D3D11GraphicsDevice& mDevice);
	~D3D11ResourceManager();

private:
	D3D11GraphicsDevice& mDevice;
};

#endif
