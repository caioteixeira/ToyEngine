
#ifdef DX12
#include "D3D12Device.h"
#include "d3dx12.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandContextManager.h"
#include <WICTextureLoader.h>


using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

D3D12GraphicsDevice::D3D12GraphicsDevice(void *window)
{
	mWindow = reinterpret_cast<HWND>(window);
	RECT rc;
	GetClientRect(mWindow, &rc);
	mWindowWidth = rc.right - rc.left;
	mWindowHeight = rc.bottom - rc.top;

	InitDevice();
	OnResize();
}

D3D12GraphicsDevice::~D3D12GraphicsDevice()
{
	auto& graphicsQueue = mCommandListManager->GetGraphicsQueue();
	graphicsQueue.Flush();
}

ComPtr<ID3DBlob> D3D12GraphicsDevice::CompileShaderFromFile(const std::wstring & filename, const D3D_SHADER_MACRO * defines, const std::string & entrypoint, const std::string & target)
{
	UINT compileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;

	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(),
		target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		SDL_Log((char*)errors->GetBufferPointer());
	}

	ThrowIfFailed(hr, "ERROR! Failed to compile shader from file!");

	return byteCode;
}

ComPtr<ID3D12RootSignature> D3D12GraphicsDevice::CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC & desc)
{
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, 
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		SDL_Log((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	hr = mDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.GetAddressOf()));

	ThrowIfFailed(hr, "ERROR! Unable to create root signature!");

	return rootSignature;
}

GraphicsBufferPtr D3D12GraphicsDevice::CreateGraphicsBuffer(const std::string & name, size_t numElements, size_t elementSize, const void * initialData)
{
	auto buffer = std::make_shared<GraphicsBuffer>();
	buffer->elementSize = elementSize;
	buffer->numElements = numElements;

	auto resource = std::make_shared<GraphicsResource>();
	resource->bufferSize = numElements * elementSize;
	resource->state = D3D12_RESOURCE_STATE_COMMON;

	buffer->resource = resource;

	//Allocate buffer
	D3D12_HEAP_PROPERTIES heapProperties;
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC desc = {};
	desc.Alignment = 0;
	desc.DepthOrArraySize = 1;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Height = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = (UINT64)resource->bufferSize;

	auto result = mDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, 
		resource->state, nullptr, IID_PPV_ARGS(&resource->buffer));
	ThrowIfFailed(result, "ERROR! Failed to initialize buffer");

	//Copies initial data
	if (initialData)
	{
		//Allocate buffer
		D3D12_HEAP_PROPERTIES UploadHeapProps;
		UploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		UploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		UploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		UploadHeapProps.CreationNodeMask = 1;
		UploadHeapProps.VisibleNodeMask = 1;

		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ComPtr<ID3D12Resource> uploadBuffer;
		auto hr = mDevice->CreateCommittedResource(&UploadHeapProps, D3D12_HEAP_FLAG_NONE,
			&desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));
		ThrowIfFailed(hr);

		auto context = mCommandListManager->AllocateContext();
		void* dest;
		ThrowIfFailed(uploadBuffer->Map(0, nullptr, &dest));
		memcpy(dest, initialData, resource->bufferSize);
		uploadBuffer->Unmap(0, nullptr);

		context->TransitionResource(resource, D3D12_RESOURCE_STATE_COPY_DEST);
		context->GetCommandList()->CopyBufferRegion(resource->buffer.Get(), 0, uploadBuffer.Get(), 0, resource->bufferSize );
		context->TransitionResource(resource, D3D12_RESOURCE_STATE_GENERIC_READ);

		context->Finish(true);
	}

	return buffer;
}

GraphicsTexturePtr D3D12GraphicsDevice::CreateTextureFromFile(const char * inFileName, int & outWidth, int & outHeight) const
{
	std::string fileStr(inFileName);
	std::string extension = fileStr.substr(fileStr.find_last_of('.'));
	const size_t cSize = strlen(inFileName) + 1;

	size_t retCount;
	std::wstring wc(cSize, L'#');
	mbstowcs_s(&retCount, &wc[0], cSize, inFileName, _TRUNCATE);
	HRESULT hr = -1;

	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subresource;
	
	ComPtr<ID3D12Resource> buffer;
	if (extension == ".png" || extension == ".bmp" || extension == ".PNG" || extension == ".BMP")
	{
		hr = DirectX::LoadWICTextureFromFile(mDevice.Get(), wc.c_str(), buffer.ReleaseAndGetAddressOf(), 
			decodedData, subresource);
	}
	else
	{
		//TODO: Use logger class
		SDL_Log("ERROR: GraphicsDriver can only load images of type DDS, PNG, or BMP.");
	}
	ThrowIfFailed(hr, "Problem Creating Texture From File");

	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(buffer.Get(), 0, 1);

	// Create the GPU upload buffer.
	ComPtr<ID3D12Resource> uploadHeap;
	hr = mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadHeap.GetAddressOf())
	);
	ThrowIfFailed(hr, "ERROR: Failed to create upload buffer");

	auto context = mCommandListManager->AllocateContext();
	
	UpdateSubresources(context->GetCommandList(), buffer.Get(), uploadHeap.Get(),
		0, 0, 1, &subresource);
	context->TransitionResource(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	context->Finish(true);

	//Create Descriptor Heap
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&descriptorHeap)));

	//Create SRV
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = buffer->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	mDevice->CreateShaderResourceView(buffer.Get(), &srvDesc, hDescriptor);

	//Fill texture object
	auto resource = std::make_shared<GraphicsResource>();
	resource->buffer = std::move(buffer);
	resource->state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	auto texture = std::make_shared<GraphicsTexture>();
	texture->resource = std::move(resource);
	texture->descriptor = std::move(descriptorHeap);

	return texture;
}

void D3D12GraphicsDevice::ClearBackBuffer(const Vector3& inColor, float alpha)
{
	auto& graphicsQueue = mCommandListManager->GetGraphicsQueue();
	graphicsQueue.WaitForFence(mPresentFences[mCurrBackBuffer]);
	assert(mPresentFences[mCurrBackBuffer] <= graphicsQueue.GetActualFenceValue());

	auto context = mCommandListManager->AllocateContext();

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	context->SetViewport(mScreenViewport);
	context->SetScissor(mScissorRect);

	// Indicate a state transition on the resource usage.
	context->TransitionResource(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear the back buffer and depth buffer.
	Color clearColor(inColor.x, inColor.y, inColor.z, alpha);
	context->ClearRenderTargetView(CurrentBackBufferView(), clearColor);
	context->ClearDepthStencilView(DepthStencilView(), 1.0f, 0);

	// Specify the buffers we are going to render to.
	context->SetRenderTarget(CurrentBackBufferView(), DepthStencilView());

	context->Finish();
}

void D3D12GraphicsDevice::Present()
{
	// swap the back and front buffers
	// Indicate a state transition on the resource usage.
	auto context = mCommandListManager->AllocateContext();
	context->TransitionResource(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mPresentFences[mCurrBackBuffer] = context->Finish();
	const auto fence = mPresentFences[mCurrBackBuffer];

	const auto hr = mSwapChain->Present(0, 0);
	ThrowIfFailed(hr, "ERROR: Failed to present");
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
}

void D3D12GraphicsDevice::InitDevice()
{
	ComPtr<ID3D12Device> device;

#if _DEBUG
	ComPtr<ID3D12Debug> debugInterface;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
	{
		debugInterface->EnableDebugLayer();

		ComPtr<ID3D12Debug1> debugInterface1;
		auto r = SUCCEEDED(debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)));
		/* if (r)
		{
			debugInterface1->SetEnableGPUBasedValidation(true);
		}
		else
		{
			SDL_Log("WARNING: Unable to enable D3D12 GPU based validation layer!");
		}*/
	}
	else
	{
		SDL_Log("WARNING: Unable to enable D3D12 debug validation layer!");
	}
#endif

	//DXGI Factory
	ThrowIfFailed(
		CreateDXGIFactory2(0, IID_PPV_ARGS(&mdxgiFactory))
	);

	//Graphics device
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

	SIZE_T maxSize = 0;

	for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != mdxgiFactory->EnumAdapters1(Idx, &adapter); ++Idx)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		if (desc.DedicatedVideoMemory > maxSize && SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
		{
			adapter->GetDesc1(&desc);
			SDL_Log("D3D12-capable hardware found:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
			maxSize = desc.DedicatedVideoMemory;
		}
	}

	if (maxSize > 0)
	{
		mDevice = device.Detach();
	}
	
	//TODO: Fallback to WARP device
	assert(mDevice != nullptr);

	InitCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
}

void D3D12GraphicsDevice::InitCommandObjects()
{
	mCommandListManager = std::make_unique<CommandContextManager>(this);
	auto & queue = mCommandListManager->GetGraphicsQueue();
	mLastFrameSubmissionFence = queue.GetActualFenceValue();
}

void D3D12GraphicsDevice::FlushCommandQueue()
{
	
}

ID3D12Resource* D3D12GraphicsDevice::CurrentBackBuffer()
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12GraphicsDevice::CurrentBackBufferView()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12GraphicsDevice::DepthStencilView()
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3D12GraphicsDevice::OnResize()
{
	assert(mDevice);
	assert(mSwapChain);

	mCommandListManager->GetGraphicsQueue().Flush();

	D3D12CommandContext* commandContext = mCommandListManager->AllocateContext();

	//Release previous resources
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	//Resize swap cahin
	auto hr = mSwapChain->ResizeBuffers(SwapChainBufferCount, mWindowWidth, mWindowHeight, 
		mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	ThrowIfFailed(hr, "ERROR: Failed to resize swap chain!");
	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < SwapChainBufferCount; i++)
	{
		hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
		ThrowIfFailed(hr, "ERROR: Failed to get swap chain buffer!");

		mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
		mPresentFences[i] = mCommandListManager->GetGraphicsQueue().GetLastCompletedFenceValue();
	}

	//Create depth stencil
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mWindowWidth;
	depthStencilDesc.Height = mWindowHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = mDepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	
	hr = mDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
	);
	ThrowIfFailed(hr, "ERROR: Unable to create DepthStencil resource!");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	commandContext->TransitionResource(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	//Finish and wait context completion
	commandContext->Finish(true);

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mWindowWidth);
	mScreenViewport.Height = static_cast<float>(mWindowHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, (LONG) mWindowWidth, (LONG) mWindowHeight };
}

void D3D12GraphicsDevice::CreateSwapChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC desc;
	desc.BufferDesc.Width = mWindowWidth;
	desc.BufferDesc.Height = mWindowHeight;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = mBackBufferFormat;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = SwapChainBufferCount;
	desc.OutputWindow = mWindow;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto& graphicsQueue = mCommandListManager->GetGraphicsQueue();
	HRESULT result = mdxgiFactory->CreateSwapChain(graphicsQueue.GetCommandQueue(), &desc, mSwapChain.GetAddressOf());
	ThrowIfFailed(result, "ERROR! Failed to create swap chain");

	//TODO: Create buffers
	mCurrBackBuffer = 0;
}

void D3D12GraphicsDevice::CreateRtvAndDsvDescriptorHeaps()
{
	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf()))
	);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}


#endif

