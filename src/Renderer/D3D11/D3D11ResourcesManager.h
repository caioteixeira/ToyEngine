#pragma once
#include "D3D11GraphicsDevice.h"

class D3D11ResourcesManager
{
public:
	D3D11ResourcesManager(D3D11GraphicsDevice& mDevice);
	~D3D11ResourcesManager();

private:
	D3D11GraphicsDevice& mDevice;
};
