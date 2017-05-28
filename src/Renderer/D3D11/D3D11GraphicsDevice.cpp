#include <iostream>
#ifdef DX11
#include "D3D11GraphicsDevice.h"
#include <WICTextureLoader.h>

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

void D3D11GraphicsDevice::SetDepthStencil(DepthStencilPtr depthStencil)
{
	mCurrentDepthStencil = depthStencil;

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

void D3D11GraphicsDevice::SetDepthStencilState(DepthStencilStatePtr inDepthStencilState) const
{
	mImmediateContext->OMSetDepthStencilState(inDepthStencilState.get(), 1);
}

bool D3D11GraphicsDevice::CompileShaderFromFile(const char* inFileName, const char* szEntryPoint, const char* szShaderModel, std::vector< char >& outCompiledShaderCode) const
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const size_t cSize = strlen(inFileName) + 1;
	size_t retCount;
	std::wstring wc(cSize, L'#');
	mbstowcs_s(&retCount, &wc[0], cSize, inFileName, _TRUNCATE);

	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlobOut = nullptr;
	hr = D3DCompileFromFile(wc.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, &pBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			static wchar_t szBuffer[4096];
			_snwprintf_s(szBuffer, 4096, _TRUNCATE,
				L"%hs",
				(char*)pErrorBlob->GetBufferPointer());
			//TODO: Use Logger
			std::cerr << szBuffer << std::endl;
//			MessageBox(nullptr, szBuffer, L"Error", MB_OK);
			pErrorBlob->Release();
			ThrowIfFailed(hr, "Shader Compilation Failed");
		}
		return false;
	}
	if (pErrorBlob)
	{
		pErrorBlob->Release();
	}

	//now copy to vector if we like it...
	if (pBlobOut)
	{
		int compiledCodeSize = pBlobOut->GetBufferSize();
		outCompiledShaderCode.resize(compiledCodeSize);
		std::memcpy(outCompiledShaderCode.data(), pBlobOut->GetBufferPointer(), compiledCodeSize);

		pBlobOut->Release();
	}

	return hr == S_OK;
}


VertexShaderPtr D3D11GraphicsDevice::CreateVertexShader(const std::vector< char >& compiledShader) const
{
	ID3D11VertexShader* toRet = nullptr;
	HRESULT hr = mDevice->CreateVertexShader(compiledShader.data(), compiledShader.size(), nullptr, &toRet);
	ThrowIfFailed(hr == S_OK, "Failure Creating Vertex Shader From Compiled Shader Code");

	return VertexShaderPtr(toRet, AutoReleaseD3D);
}


PixelShaderPtr D3D11GraphicsDevice::CreatePixelShader(const std::vector< char >& compiledShader) const
{
	ID3D11PixelShader* toRet = nullptr;
	HRESULT hr = mDevice->CreatePixelShader(compiledShader.data(), compiledShader.size(), nullptr, &toRet);
	ThrowIfFailed(hr == S_OK, "Failure Creating Pixel Shader From Compiled Shader Code");
	return PixelShaderPtr(toRet, AutoReleaseD3D);
}

InputLayoutPtr D3D11GraphicsDevice::CreateInputLayout(const InputLayoutElement* elements, int numElements, const std::vector<char>& compiledVertexShader) const
{
	ID3D11InputLayout* toRet = nullptr;
	HRESULT hr = mDevice->CreateInputLayout(
		elements, numElements,
		compiledVertexShader.data(), compiledVertexShader.size(),
		&toRet);
	ThrowIfFailed(hr, "Failure Creating Vertex Layout");

	return InputLayoutPtr(toRet, AutoReleaseD3D);
}

GraphicsBufferPtr D3D11GraphicsDevice::CreateGraphicsBuffer(const void* rawData, int rawDataSize, EBindflags bindFlags, ECPUAccessFlags cpuAccessFlags, EGraphicsBufferUsage bufferUsage) const
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof bd);

	bd.Usage = static_cast<D3D11_USAGE>(bufferUsage);
	bd.ByteWidth = rawDataSize;

	bd.BindFlags = bindFlags;

	bd.CPUAccessFlags = cpuAccessFlags;
	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof initialData);
	initialData.pSysMem = rawData;

	ID3D11Buffer* ret = nullptr;
	HRESULT hr = mDevice->CreateBuffer(&bd, rawData ? &initialData : nullptr, &ret);
	ThrowIfFailed(hr, "Problem Creating Vertex Buffer!");

	return GraphicsBufferPtr(ret, AutoReleaseD3D);
}

GraphicsTexturePtr D3D11GraphicsDevice::CreateTextureFromFile(const char* path, int& outWidth, int& outHeight)
{
	std::string fileStr(path);
	std::string extension = fileStr.substr(fileStr.find_last_of('.'));
	const size_t cSize = strlen(path) + 1;

	size_t retCount;
	std::wstring wc(cSize, L'#');
	mbstowcs_s(&retCount, &wc[0], cSize, path, _TRUNCATE);
	ID3D11ShaderResourceView* toRet = nullptr;
	ID3D11Resource* texture = nullptr;
	HRESULT hr = -1;
	
	if (extension == ".png" || extension == ".bmp" || extension == ".PNG" || extension == ".BMP")
	{
		hr = DirectX::CreateWICTextureFromFile(mDevice, wc.c_str(), &texture, &toRet);
	}
	else
	{
		//TODO: Use logger class
		std::cerr << "GraphicsDriver can only load images of type DDS, PNG, or BMP." << std::endl;
	}
	ThrowIfFailed(hr == S_OK, "Problem Creating Texture From File");


	CD3D11_TEXTURE2D_DESC textureDesc;
	((ID3D11Texture2D*)texture)->GetDesc(&textureDesc);
	outWidth = textureDesc.Width;
	outHeight = textureDesc.Height;

	return GraphicsTexturePtr(toRet, AutoReleaseD3D);
}

DepthStencilPtr D3D11GraphicsDevice::CreateDepthStencil(int inWidth, int inHeight) const
{
	ID3D11Texture2D* depthStencilTexture;
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = inWidth;
	descDepth.Height = inHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT hr = mDevice->CreateTexture2D(&descDepth, nullptr, &depthStencilTexture);
	ThrowIfFailed(hr == S_OK, "Problem Creating Depth Stencil");

	ID3D11DepthStencilView* toRet = nullptr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mDevice->CreateDepthStencilView(depthStencilTexture, &descDSV, &toRet);
	ThrowIfFailed(hr == S_OK, "Problem Creating Depth Stencil");

	return DepthStencilPtr(toRet, AutoReleaseD3D);
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

DepthStencilStatePtr D3D11GraphicsDevice::CreateDepthStencilState(bool inDepthTestEnable, EComparisonFunc inDepthComparisonFunction) const
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = inDepthTestEnable;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = static_cast< D3D11_COMPARISON_FUNC >(inDepthComparisonFunction);

	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* toRet = nullptr;
	HRESULT hr = mDevice->CreateDepthStencilState(&dsDesc, &toRet);
	ThrowIfFailed(hr == S_OK, "Problem Creating Depth Stencil");

	return DepthStencilStatePtr(toRet, AutoReleaseD3D);
}

BlendStatePtr D3D11GraphicsDevice::CreateBlendState(bool inEnableBlend) const
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.RenderTarget[0].BlendEnable = inEnableBlend;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	ID3D11BlendState* toRet;
	auto hr = mDevice->CreateBlendState(&desc, &toRet);
	ThrowIfFailed(hr == S_OK, "Problem Creating Blend State");

	return BlendStatePtr(toRet, AutoReleaseD3D);
}

SamplerStatePtr D3D11GraphicsDevice::CreateSamplerState() const
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* toRet = nullptr;
	HRESULT hr = mDevice->CreateSamplerState(&sampDesc, &toRet);
	ThrowIfFailed(hr == S_OK, "Failure Creating Sampler State");

	return SamplerStatePtr(toRet, AutoReleaseD3D);
}

void D3D11GraphicsDevice::SetRasterizerState(RasterizerStatePtr rasterizerState) const
{
	mImmediateContext->RSSetState(rasterizerState.get());
}

void D3D11GraphicsDevice::SetBlendState(BlendStatePtr inBlendState) const
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	mImmediateContext->OMSetBlendState(inBlendState.get(), blendFactor, 0xffffffff);
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

void D3D11GraphicsDevice::SetPSSamplerState(SamplerStatePtr inSamplerState, int inStartSlot) const
{
	auto sampler = inSamplerState.get();
	mImmediateContext->PSSetSamplers(inStartSlot, 1, &sampler);
}

void D3D11GraphicsDevice::Present() const
{
	mSwapChain->Present(0, 0);
}

#endif