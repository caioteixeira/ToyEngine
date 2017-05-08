#ifdef DX11
#include "D3D11GraphicsDevice.h"

void AutoReleaseD3D(ID3D11DeviceChild* inResource)
{
	inResource->Release();
}

D3D11GraphicsDevice::D3D11GraphicsDevice(void *inWindow)
{
	HWND window = reinterpret_cast<HWND>(inWindow);
	RECT rc;
	GetClientRect(window, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	CreateDevice();
	CreateSwapChain(width, height, window);
	mBackBufferRenderTarget = CreateBackBufferRenderTargetView();

	mWindowWidth = width;
	mWindowHeight = height;
	SetViewPort(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

	SetRasterizerState(CreateRasterizerState(EFM_Wireframe));

	SetPrimitiveTopology(EPT_TriangleList);

	SetRenderTarget(GetBackBufferRenderTarget());
}


D3D11GraphicsDevice::~D3D11GraphicsDevice()
{
}

void D3D11GraphicsDevice::CreateDevice()
{
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = S_OK;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &mDevice, &featureLevel, &mImmediateContext);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &mDevice, &featureLevel, &mImmediateContext);
		}

		if (SUCCEEDED(hr))
		{
			return;
		}
	}

	//TODO: Log error creating device
}

HRESULT D3D11GraphicsDevice::CreateSwapChain(UINT backBufferWidth, UINT backBufferHeight, HWND window)
{
	HRESULT hr;

	//Obtain DXGI Factory
	IDXGIFactory1 * dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = mDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
		if(SUCCEEDED(hr))
		{
			IDXGIAdapter * adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if(SUCCEEDED(hr))
			{
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&dxgiFactory));
				adapter->Release();
			}
		}
	}

	if( FAILED(hr))
	{
		return hr;
	}

	//Create swap cahin
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	
	if (dxgiFactory2)
	{
		// DirectX 11.1 or later
		hr = mDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&mDevice1));
		if (SUCCEEDED(hr))
		{
			(void)mDevice->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&mImmediateContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = backBufferWidth;
		sd.Height = backBufferWidth;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(mDevice, window, &sd, nullptr, nullptr, &mSwapChain1);
		if (SUCCEEDED(hr))
		{
			hr = mSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&mSwapChain));
		}

		dxgiFactory2->Release();
	}
	else
	{
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = backBufferWidth;
		sd.BufferDesc.Height = backBufferHeight;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = window;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgiFactory->CreateSwapChain(mDevice, &sd, &mSwapChain);
	}

	dxgiFactory->Release();

	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

RenderTargetPtr D3D11GraphicsDevice::CreateBackBufferRenderTargetView() const
{
	HRESULT hr;
	//Create a render target view
	ID3D11Texture2D* backBuffer = nullptr;

	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
	if(FAILED(hr))
	{
		return nullptr;
	}

	ID3D11RenderTargetView* view;
	hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &view);
	if(FAILED(hr))
	{
		return nullptr;
	}

	return RenderTargetPtr(view, AutoReleaseD3D);
}

void D3D11GraphicsDevice::SetRenderTarget(RenderTargetPtr renderTargetPtr)
{
	mCurrentRenderTarget = renderTargetPtr;

	auto renderTarget = mCurrentRenderTarget.get();
	mImmediateContext->OMSetRenderTargets(1, &renderTarget, mCurrentDepthStencil.get());
}

void D3D11GraphicsDevice::SetViewPort(float x, float y, float width, float height) const
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mImmediateContext->RSSetViewports(1, &viewport);
}

void D3D11GraphicsDevice::SetPrimitiveTopology(EPrimitiveTopology topology) const
{
	mImmediateContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(topology));
}

RasterizerStatePtr D3D11GraphicsDevice::CreateRasterizerState(EFillMode fillMode) const
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof desc);

	desc.FillMode = static_cast<D3D11_FILL_MODE>(fillMode);

	//TODO: Set cull mode
	desc.CullMode = D3D11_CULL_NONE;

	ID3D11RasterizerState* state;
	auto hr = mDevice->CreateRasterizerState(&desc, &state);
	return RasterizerStatePtr(state, AutoReleaseD3D);
}

void D3D11GraphicsDevice::SetRasterizerState(RasterizerStatePtr rasterizerState) const
{
	mImmediateContext->RSSetState(rasterizerState.get());
}

void D3D11GraphicsDevice::ClearBackBuffer(const Vector3& color, float alpha) const
{
	ClearRenderTarget(mBackBufferRenderTarget, color, alpha);
}

void D3D11GraphicsDevice::ClearRenderTarget(RenderTargetPtr renderTarget, const Vector3& inColor, float alpha) const
{
	float color[4];
	color[0] = inColor.x;
	color[1] = inColor.y;
	color[2] = inColor.z;
	color[3] = alpha;

	mImmediateContext->ClearRenderTargetView(renderTarget.get(), color);
}

void D3D11GraphicsDevice::Present() const
{
	mSwapChain->Present(0, 0);
}

#endif