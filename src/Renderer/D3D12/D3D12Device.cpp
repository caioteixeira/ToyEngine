
#ifdef DX12
#include "D3D12Device.h"

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

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

void D3D12GraphicsDevice::Present() const
{
	
}

void D3D12GraphicsDevice::InitDevice()
{
	HRESULT result;
#if defined(DEBUG) || defined(_DEBUG)
{
	ComPtr<ID3D12Debug> debugController;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	ThrowIfFailed(result, "Could not initialize debug layer!");

	debugController->EnableDebugLayer();
}
#endif

	result = CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory));
	ThrowIfFailed(result, "Failed to initialize DXGI Factory!");
	
	//Create hardware device
	result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));

	//Fallback to WARP
	if(FAILED(result))
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		result = mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		ThrowIfFailed(result, "Failed to get warp adapter");

		result = D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
		ThrowIfFailed(result, "Failed to create device!");
	}

	result = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	ThrowIfFailed(result, "Failed to create fence!");

	mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//TODO: Log adapters

	InitCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
}

void D3D12GraphicsDevice::InitCommandObjects()
{
	HRESULT result;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	result = mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
	ThrowIfFailed(result, "Failed to create command queue!");

	result = mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf()));
	ThrowIfFailed(result, "Failed to create command allocator!");

	result = mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mDirectCmdListAlloc.Get(), 
		nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf()));
	ThrowIfFailed(result, "Failed to create command list!");

	//Start off closed so we can reset it on first use
	mCommandList->Close();
}

void D3D12GraphicsDevice::FlushCommandQueue()
{
	mCurrentFence++;

	ThrowIfFailed(
		mCommandQueue->Signal(mFence.Get(), mCurrentFence),
		"Failed to signal fence!"
	);

	//Wait until GPU has completed commands up to this point
	if(mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		ThrowIfFailed(
			mFence->SetEventOnCompletion(mCurrentFence, eventHandle)
		);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
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
	assert(mDirectCmdListAlloc);

	//Flush queue and release resources
	FlushCommandQueue();

	ThrowIfFailed(
		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr)
	);

	for(int i = 0; i < SwapChainBufferCount; ++i)
	{
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	//Resize swap chain
	ThrowIfFailed(
		mSwapChain->ResizeBuffers(SwapChainBufferCount, mWindowWidth, mWindowHeight, 
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)
	);
	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for(UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(
			mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]))
		);

		mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	//Create depth/stencil buffer views
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mWindowWidth;
	depthStencilDesc.Height = mWindowHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(
		mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
		)
	);

	//Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	//Transition the resources from its initial state to be used as a depth buffer
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	//Execute the resize commands
	ThrowIfFailed(
		mCommandList->Close()
	);
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mWindowWidth);
	mScreenViewport.Height = static_cast<float>(mWindowHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, static_cast<int>(mWindowWidth), static_cast<int>(mWindowHeight) };
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

	HRESULT result = mdxgiFactory->CreateSwapChain(mCommandQueue.Get(), &desc, mSwapChain.GetAddressOf());
}

void D3D12GraphicsDevice::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())), 
		"Failed to CreateDescriptor heap!"
	);


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())),
		"Failed to Create Descriptor Heap!"
	);
}


#endif

