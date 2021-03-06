#include <easy/profiler.h>

#include "D3D12Device.h"
#include "d3dx12.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandContextManager.h"
#include "WICTextureLoader12.h"

#include "../../EngineCore.h"


using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;
using namespace Engine;

D3D12GraphicsDevice::D3D12GraphicsDevice(void* window)
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

ComPtr<ID3DBlob> D3D12GraphicsDevice::CompileShaderFromFile(const std::wstring& filename,
                                                            const D3D_SHADER_MACRO* defines,
                                                            const std::string& entrypoint, const std::string& target)
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
        Logger::DebugLogError((char*)errors->GetBufferPointer());
    }

    ThrowIfFailed(hr, "ERROR! Failed to compile shader from file!");

    return byteCode;
}

ComPtr<ID3D12RootSignature> D3D12GraphicsDevice::CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC& desc)
{
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,
                                             serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        Logger::DebugLogError((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ComPtr<ID3D12RootSignature> rootSignature = nullptr;

    hr = mDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
                                      IID_PPV_ARGS(rootSignature.GetAddressOf()));

    ThrowIfFailed(hr, "ERROR! Unable to create root signature!");

    return rootSignature;
}

ComPtr<ID3D12DescriptorHeap> D3D12GraphicsDevice::CreateDescriptorHeap(enum D3D12_DESCRIPTOR_HEAP_TYPE heapType,
                                                                       enum D3D12_DESCRIPTOR_HEAP_FLAGS isShaderVisible,
                                                                       int numDescriptors) const
{
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = numDescriptors;
    srvHeapDesc.Type = heapType;
    srvHeapDesc.Flags = isShaderVisible;
    ThrowIfFailed(mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&descriptorHeap)));

    return std::move(descriptorHeap);
}

void D3D12GraphicsDevice::ClearBackBuffer(const Vector3& inColor, float alpha)
{
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

    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    EASY_BLOCK("Wait to present")
    auto& graphicsQueue = mCommandListManager->GetGraphicsQueue();
    graphicsQueue.WaitForFence(mPresentFences[mCurrBackBuffer]);
    assert(mPresentFences[mCurrBackBuffer] <= graphicsQueue.GetLastCompletedFenceValue());
    EASY_END_BLOCK

    const auto hr = mSwapChain->Present(0, 0);
    ThrowIfFailed(hr, "ERROR: Failed to present");
}

UINT D3D12GraphicsDevice::GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
    switch (type)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        return mCbvSrvUavDescriptorSize;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        //TODO: Implement sampler size
        return -1;
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        return mRtvDescriptorSize;
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        return mDsvDescriptorSize;
    default:
        return -1;
    }
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
        const auto r = SUCCEEDED(debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)));
        if (r)
        {
            debugInterface1->SetEnableGPUBasedValidation(false);
        }
        else
        {
            Logger::DebugLogError("WARNING: Unable to enable D3D12 GPU based validation layer!");
        }
    }
    else
    {
        Logger::DebugLogError("WARNING: Unable to enable D3D12 debug validation layer!");
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

        if (desc.DedicatedVideoMemory > maxSize && SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device))))
        {
            adapter->GetDesc1(&desc);
            Logger::DebugLog("D3D12-capable hardware found:  %s (%u MB)\n", desc.Description,
                             desc.DedicatedVideoMemory >> 20);
            maxSize = desc.DedicatedVideoMemory;
        }
    }

    if (maxSize > 0)
    {
        mDevice = device.Detach();
    }

    //TODO: Fallback to WARP device
    assert(mDevice != nullptr);

    mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    InitCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();
}

void D3D12GraphicsDevice::InitCommandObjects()
{
    mCommandListManager = std::make_unique<CommandContextManager>(this);
    auto& queue = mCommandListManager->GetGraphicsQueue();
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
        mPresentFences[i] = mCommandListManager->GetGraphicsQueue().GetActualFenceValue();
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

    auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    hr = mDevice->CreateCommittedResource(
        &heapProperties,
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

    mScissorRect = {0, 0, (LONG)mWindowWidth, (LONG)mWindowHeight};
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
