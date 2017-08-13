
#ifdef DX12
#include "D3D12Device.h"
#include "d3dx12.h"

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

}

void D3D12GraphicsDevice::ClearBackBuffer(const Vector3& inColor, float alpha)
{
	/*
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Clear the back buffer and depth buffer.
	float clearColor[4];
	clearColor[0] = inColor.x;
	clearColor[1] = inColor.y;
	clearColor[2] = inColor.z;
	clearColor[3] = alpha;

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), clearColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
	*/
}

void D3D12GraphicsDevice::CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator)
{
	auto result = mDevice->CreateCommandAllocator(type,
		IID_PPV_ARGS(allocator));
	ThrowIfFailed(result, "Failed to create command allocator!");

	if(type == D3D12_COMMAND_LIST_TYPE_COPY)
	{
		*allocator = mCopyQueue->GetAllocator();
	}
	else
	{
		*allocator = mGraphicsQueue->GetAllocator();
	}


	result = mDevice->CreateCommandList(1, type, *allocator,
		nullptr, IID_PPV_ARGS(list));
	ThrowIfFailed(result, "Failed to create command list!");
}

void D3D12GraphicsDevice::Present()
{
	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
}

void D3D12GraphicsDevice::InitDevice()
{
	Microsoft::WRL::ComPtr<ID3D12Device> device;

#if _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
	{
		debugInterface->EnableDebugLayer();
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

	//InitCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
}

void D3D12GraphicsDevice::InitCommandObjects()
{
	mCopyQueue = std::make_unique<D3D12CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY, mDevice.Get());
	mGraphicsQueue = std::make_unique<D3D12CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT, mDevice.Get());
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

	//TODO: Flush command queues

	//TODO: Get from context pool
	D3D12CommandContext commandContext(D3D12_COMMAND_LIST_TYPE_DIRECT, this);

	//Release previous resources
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	//Resize swap cahin
	auto hr = mSwapChain->ResizeBuffers(SwapChainBufferCount, mWindowWidth, mWindowHeight, 
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	ThrowIfFailed(hr, "ERROR: Failed to resize swap chain!");
	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < SwapChainBufferCount; i++)
	{
		hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
		ThrowIfFailed(hr, "ERROR: Failed to get swap chain buffer!");

		mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
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
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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
	commandContext.BeginResourceTransition(mDepthStencilBuffer.Get(), 
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	//Finish and wait context completion
	commandContext.Finish(true);

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mWindowWidth);
	mScreenViewport.Height = static_cast<float>(mWindowHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, mWindowWidth, mWindowHeight };
}

void D3D12GraphicsDevice::CreateSwapChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC desc;
	desc.BufferDesc.Width = mWindowWidth;
	desc.BufferDesc.Height = mWindowHeight;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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

	HRESULT result = mdxgiFactory->CreateSwapChain(mGraphicsQueue->GetCommandQueue(), &desc, reinterpret_cast<IDXGISwapChain1**>(mSwapChain.GetAddressOf));
	ThrowIfFailed(result, "ERROR! Failed to create swap chain");

	//TODO: Create buffers
	mCurrBackBuffer = mSwapChain->GetCurrentBackBufferIndex();
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

